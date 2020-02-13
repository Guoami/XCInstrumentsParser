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
    [allocInstrument._topLevelContexts[2] display];
    XRManagedEventArrayController *arrayController = GetVariable(GetVariable(allocInstrument, _objectListController), _ac);
    
    NSByteCountFormatter *byteFormatter = [[NSByteCountFormatter alloc] init];
    byteFormatter.countStyle = NSByteCountFormatterCountStyleBinary;
    NSMutableDictionary<NSNumber *, NSNumber *> *memoryStamps = [NSMutableDictionary dictionary];
    NSInteger totalSize = 0;
    for (XRObjectAllocEvent *event in arrayController.arrangedObjects) {
        NSNumber *time = @(event.timestamp / NSEC_PER_SEC);
        totalSize += event.size;
         
        memoryStamps[time] = [NSNumber numberWithInteger: totalSize];
    }
    
    NSArray<NSNumber *> *sortedTime = [memoryStamps.allKeys sortedArrayUsingComparator:^(NSNumber *time1, NSNumber *time2) {
        return [time1 compare:time2];
    }];
    
    NSMutableArray<NSString *> *resultArray = @[].mutableCopy;
    for (NSNumber *time in sortedTime) {
        NSString *size = [byteFormatter stringForObjectValue:memoryStamps[time]];
        [resultArray addObject:[NSString stringWithFormat:@"%lds %@", time.integerValue, size]];
    }
    return [resultArray copy];
}


void handleAllocationsTemplate(XRInstrument *instrument) {
    
}

@end
