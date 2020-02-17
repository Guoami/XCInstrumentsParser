//
//  ActivityMonitorTemplateParser.m
//  XCInstrumentsParser
//
//  Created by Ruslan Nikolayev on 2/17/20.
//  Copyright © 2020 Ruslan Nikolayev. All rights reserved.
//

#import "ActivityMonitorTemplateParser.h"
#import "Instruments.h"
#import <objc/runtime.h>

@implementation ActivityMonitorTemplateParser

+(NSArray<NSString *> *) parseCPUWithInstrument: (XRInstrument *) instrument {
    XRObjectAllocInstrument *allocInstrument = (XRObjectAllocInstrument *)instrument;
    
    // ? contexts: ?
    // Show ?:
//    XRContext *context = allocInstrument._topLevelContexts[2];
//    [context display];
    
//    XROAArrayController *arrayController = GetVariable(allocInstrument, _summaryController);
//    XROAEventSummary *summary = [[arrayController arrangedObjects] firstObject];
//
//    for (XRObjectAllocEvent *event in arrayController.arrangedObjects) {
//        NSNumber *time = @(event.timestamp / NSEC_PER_SEC);
////        printf("\n[DETAILS] new delta: %d\n", event.delta);
////        totalSize += event.delta;
//        if ([memoryStamps.allKeys containsObject:time]) {
//            NSInteger summ = memoryStamps[time].integerValue + event.size;
//            memoryStamps[time] = [NSNumber numberWithInteger: summ];
//        } else {
//            memoryStamps[time] = [NSNumber numberWithInteger: event.size];
//        }
//    }
//
//    NSArray<NSNumber *> *sortedTime = [memoryStamps.allKeys sortedArrayUsingComparator:^(NSNumber *time1, NSNumber *time2) {
//        return [time1 compare:time2];
//    }];
    
    NSMutableArray<NSString *> *resultArray = @[].mutableCopy;
//    for (NSNumber *time in sortedTime) {
//        NSString *size = [byteFormatter stringForObjectValue:memoryStamps[time]];
//        [resultArray addObject:[NSString stringWithFormat:@"%lds %@", time.integerValue, size]];
//    }
    return [resultArray copy];
}

@end
