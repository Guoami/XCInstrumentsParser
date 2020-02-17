//
//  ActivityMonitorTemplateParser.h
//  XCInstrumentsParser
//
//  Created by Ruslan Nikolayev on 2/17/20.
//  Copyright © 2020 Ruslan Nikolayev. All rights reserved.
//

#import <Foundation/Foundation.h>

@class XRInstrument;

@interface ActivityMonitorTemplateParser : NSObject

+(NSArray<NSString *> *) parseCPUWithInstrument: (XRInstrument *) instrument;

@end
