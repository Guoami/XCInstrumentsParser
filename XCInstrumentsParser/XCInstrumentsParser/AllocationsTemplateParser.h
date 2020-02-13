//
//  AllocationsTemplateParser.h
//  XCInstrumentsParser
//
//  Created by Ruslan Nikolayev on 2/13/20.
//  Copyright © 2020 Ruslan Nikolayev. All rights reserved.
//

#import <Foundation/Foundation.h>

@class XRInstrument;

@interface AllocationsTemplateParser : NSObject

+(NSArray<NSString *> *) parseAllocationsWithInstrument: (XRInstrument *) instrument;

@end
