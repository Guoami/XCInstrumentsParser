//
//  AllocationsTemplateParser.m
//  XCInstrumentsParser
//
//  Created by Ruslan Nikolayev on 2/13/20.
//  Copyright © 2020 Ruslan Nikolayev. All rights reserved.
//

#import "AllocationsTemplateParser.h"
#import "Instruments.h"

@implementation AllocationsTemplateParser

+(NSArray<NSString *> *) parseAllocationsWithInstrument: (XRInstrument *) instrument {
    XRObjectAllocInstrument *allocInstrument = (XRObjectAllocInstrument *)instrument;
    
    // 4 contexts: Statistics, Call Trees, Allocations List, Generations.
    // Show staticstics:
    XRContext *context = allocInstrument._topLevelContexts[0];
    [context display];
    
    // Summary controller is the bottom left section which contains all the data abour selected timeframe
    XROAArrayController *arrayController = GetVariable(allocInstrument, _summaryController);
    // Formats bytes for us, neat
    NSByteCountFormatter *byteFormatter = [[NSByteCountFormatter alloc] init];
    byteFormatter.countStyle = NSByteCountFormatterCountStyleBinary;
    
    XRRun *lastRun = [instrument.allRuns lastObject];
    XRTime durationSeconds = lastRun.timeRange.length / NSEC_PER_SEC;
    NSMutableArray<NSString *> *resultArray = @[].mutableCopy;
    // We start from 1st second because there is no information about mem consumption on the start
    for (XRTime i = 1; i < durationSeconds; i++) {
        // Create a time range so that Instrument knows which statistics should be shown
        XRTimeRange range = { .start = 0, .length = i * NSEC_PER_SEC };
        // Apply time range
        [allocInstrument setSelectedTimeRange:range];
        
        // Get the first row of summary data (All Heap & Anonymous VM)
        XROAEventSummary *summary = [arrayController.arrangedObjects firstObject];
        // Summary can be zero in case there is not enough stats
        if (summary != nil) {
            long long bytes = GetVariable(summary, activeBytes);
            
            NSString *formattedBytes = [byteFormatter stringForObjectValue:[NSNumber numberWithLongLong:bytes]];
            NSString *result = [NSString stringWithFormat:@"%llus %@", i, formattedBytes];
            [resultArray addObject:result];
        }
    }
    return [resultArray copy];
}

@end
