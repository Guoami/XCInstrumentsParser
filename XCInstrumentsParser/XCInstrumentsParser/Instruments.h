//
//  Instruments.h
//  XCInstrumentsParser
//
//  Created by Ruslan Nikolayev on 2/13/20.
//  Copyright © 2020 Ruslan Nikolayev. All rights reserved.
//

#import <AppKit/AppKit.h>

#import <objc/runtime.h>

// These are required for fetching private variables using objc runtime
#define CastVariable(object, name, type) (*(type *)(void *)&((char *)(__bridge void *)object)[ivar_getOffset(class_getInstanceVariable(object_getClass(object), #name))])
#define GetVariable(object, name) CastVariable(object, name, id const)

// Logging util that helps with finding private variables/methods
static void introscope(char *label, Class classInstance) {
    printf("**\n%s-----------\n", label);
    
    unsigned int count = 0;
    Ivar *ivars = class_copyIvarList(classInstance, &count);
    printf("\nIvars:\n");
    for (int i=0;i<count;i++) {
        Ivar ivar = ivars[i];
        const char *name = ivar_getName(ivar);
        const char *type = ivar_getTypeEncoding(ivar);
        printf("\n%s: %s\n", type, name);
    }
    
    count = 0;
    Method *methods = class_copyMethodList(classInstance, &count);
    printf("\nMethods:\n");
    for (int i=0;i<count;i++) {
        Method method = methods[i];
        struct objc_method_description *info = method_getDescription(method);
        
        const char *name = sel_getName(info->name);
        const char *types = info->types;
        printf("\n%s %s\n", name, types);
    }
    
    printf("**\n-----------\n");
}

NSString *PFTDeveloperDirectory(void);
void DVTInitializeSharedFrameworks(void);
BOOL PFTLoadPlugins(void);
void PFTClosePlugins(void);

@interface DVTDeveloperPaths : NSObject
+ (NSString *)applicationDirectoryName;
+ (void)initializeApplicationDirectoryName:(NSString *)name;
@end

@interface XRInternalizedSettingsStore : NSObject
+ (NSDictionary *)internalizedSettings;
+ (void)configureWithAdditionalURLs:(NSArray *)urls;
@end

@interface XRCapabilityRegistry : NSObject
+ (instancetype)applicationCapabilities;
- (void)registerCapability:(NSString *)capability versions:(NSRange)versions;
@end

typedef UInt64 XRTime; // in nanoseconds
typedef struct { XRTime start, length; } XRTimeRange;

@interface XRRun : NSObject
- (SInt64)runNumber;
- (NSString *)displayName;
- (XRTimeRange)timeRange;
- (double) startTime;
@end

@interface PFTInstrumentType : NSObject
- (NSString *)uuid;
- (NSString *)name;
- (NSString *)category;
@end

@protocol XRInstrumentViewController;

@interface XRInstrument : NSObject
- (PFTInstrumentType *)type;
- (id<XRInstrumentViewController>)viewController;
- (void)setViewController:(id<XRInstrumentViewController>)viewController;
- (NSArray<XRRun *> *)allRuns;
- (XRRun *)currentRun;
- (void)setCurrentRun:(XRRun *)run;
@end

@interface PFTInstrumentList : NSObject
- (NSArray<XRInstrument *> *)allInstruments;
@end

@interface XRTrace : NSObject
- (PFTInstrumentList *)allInstrumentsList;
@end

@interface XRDevice : NSObject
- (NSString *)deviceIdentifier;
- (NSString *)deviceDisplayName;
- (NSString *)deviceDescription;
- (NSString *)productType;
- (NSString *)productVersion;
- (NSString *)buildVersion;
@end

@interface PFTProcess : NSObject
- (NSString *)bundleIdentifier;
- (NSString *)processName;
- (NSString *)displayName;
@end

@interface PFTTraceDocument : NSDocument
- (XRTrace *)trace;
- (XRDevice *)targetDevice;
- (PFTProcess *)defaultProcess;
@end

@interface PFTDocumentController : NSDocumentController
@end

@protocol XRContextContainer;

@interface XRContext : NSObject
- (NSString *)label;
- (id<NSCoding>)value;
- (id<XRContextContainer>)container;
- (instancetype)parentContext;
- (instancetype)rootContext;
- (void)display;
@end

@protocol XRContextContainer <NSObject>
- (XRContext *)contextRepresentation;
- (NSArray<XRContext *> *)siblingsForContext:(XRContext *)context;
- (void)displayContext:(XRContext *)context;
@end

@protocol XRFilteredDataSource <NSObject>
@end

@protocol XRSearchTarget <NSObject>
@end

@protocol XRCallTreeDataSource <NSObject>
@end

@protocol XRAnalysisCoreViewSubcontroller <XRContextContainer, XRFilteredDataSource>
@end

typedef NS_ENUM(SInt32, XRAnalysisCoreDetailViewType) {
    XRAnalysisCoreDetailViewTypeProjection = 1,
    XRAnalysisCoreDetailViewTypeCallTree = 2,
    XRAnalysisCoreDetailViewTypeTabular = 3,
};

@interface XRAnalysisCoreDetailNode : NSObject
- (instancetype)firstSibling;
- (instancetype)nextSibling;
- (XRAnalysisCoreDetailViewType)viewKind;
@end

@class XRAnalysisCoreProjectionViewController, XRAnalysisCoreCallTreeViewController, XRAnalysisCoreTableViewController;

@interface XRAnalysisCoreDetailViewController : NSViewController <XRAnalysisCoreViewSubcontroller> {
    XRAnalysisCoreDetailNode *_firstNode;
    XRAnalysisCoreProjectionViewController *_projectionViewController;
    XRAnalysisCoreCallTreeViewController *_callTreeViewController;
    XRAnalysisCoreTableViewController *_tabularViewController;
}
- (void)restoreViewState;
@end

XRContext *XRContextFromDetailNode(XRAnalysisCoreDetailViewController *detailController, XRAnalysisCoreDetailNode *detailNode);

@protocol XRInstrumentViewController <NSObject>
- (id<XRContextContainer>)detailContextContainer;
- (id<XRFilteredDataSource>)detailFilteredDataSource;
- (id<XRSearchTarget>)detailSearchTarget;
- (void)instrumentDidChangeSwitches;
- (void)instrumentChangedTableRequirements;
- (void)instrumentWillBecomeInvalid;
@end

@interface XRAnalysisCoreStandardController : NSObject <XRInstrumentViewController>
- (instancetype)initWithInstrument:(XRInstrument *)instrument document:(PFTTraceDocument *)document;
@end

@interface XRAnalysisCoreProjectionViewController : NSViewController <XRSearchTarget>
@end

@interface PFTCallTreeNode : NSObject
- (NSString *)libraryName;
- (NSString *)symbolName;
- (UInt64)address;
- (NSArray *)symbolNamePath; // Call stack
- (instancetype)root;
- (instancetype)parent;
- (NSArray *)children;
- (SInt32)numberChildren;
- (SInt32)terminals; // An integer value of this node, such as self running time in millisecond.
- (SInt32)count; // Total value of all nodes of the subtree whose root node is this node. It means that if you increase terminals by a value, count will also be increased by the same value, and that the value of count is calculated automatically and you connot modify it.
- (UInt64)weightCount; // Count of different kinds of double values;
- (Float64)selfWeight:(UInt64)index; // A double value similar to terminal at the specific index.
- (Float64)weight:(UInt64)index; // A double value similar to count at the specific index. The difference is that you decide how weigh should be calculated.
- (Float64)selfCountPercent; // self.terminal / root.count
- (Float64)totalCountPercent; // self.count / root.count
- (Float64)parentCountPercent; // parent.count / root.count
- (Float64)selfWeightPercent:(UInt64)index; // self.selfWeight / root.weight
- (Float64)totalWeightPercent:(UInt64)index; // self.weight / root.weight
- (Float64)parentWeightPercent:(UInt64)index; // parent.weight / root.weight
@end

@interface XRBacktraceRepository : NSObject
- (PFTCallTreeNode *)rootNode;
@end

@interface XRMultiProcessBacktraceRepository : XRBacktraceRepository
@end

@interface XRAnalysisCoreCallTreeViewController : NSViewController <XRFilteredDataSource, XRCallTreeDataSource> {
    XRBacktraceRepository *_backtraceRepository;
}
@end

typedef void XRAnalysisCoreReadCursor;
typedef union {
    UInt32 uint32;
    UInt64 uint64;
    UInt32 iid;
} XRStoredValue;

@interface XRAnalysisCoreValue : NSObject
- (XRStoredValue)storedValue;
- (id)objectValue;
@end

BOOL XRAnalysisCoreReadCursorNext(XRAnalysisCoreReadCursor *cursor);
SInt64 XRAnalysisCoreReadCursorColumnCount(XRAnalysisCoreReadCursor *cursor);
XRStoredValue XRAnalysisCoreReadCursorGetStored(XRAnalysisCoreReadCursor *cursor, UInt8 column);
BOOL XRAnalysisCoreReadCursorGetValue(XRAnalysisCoreReadCursor *cursor, UInt8 column, XRAnalysisCoreValue * __strong *pointer);

@interface XREngineeringTypeFormatter : NSFormatter
@end

@interface XRAnalysisCoreFullTextSearchSpec : NSObject
- (XREngineeringTypeFormatter *)formatter;
@end

@interface XRAnalysisCoreTableQuery : NSObject
- (XRAnalysisCoreFullTextSearchSpec *)fullTextSearchSpec;
@end

@interface XRAnalysisCoreRowArray : NSObject {
    XRAnalysisCoreTableQuery *_filter;
}
@end

@interface XRAnalysisCorePivotArrayAccessor : NSObject
- (UInt64)rowInDimension:(UInt8)dimension closestToTime:(XRTime)time intersects:(SInt8 *)intersects;
- (void)readRowsStartingAt:(UInt64)index dimension:(UInt8)dimension block:(void (^)(XRAnalysisCoreReadCursor *cursor))block;
@end

@interface XRAnalysisCorePivotArray : NSObject
- (XRAnalysisCoreRowArray *)source;
- (UInt64)count;
- (void)access:(void (^)(XRAnalysisCorePivotArrayAccessor *accessor))block;
@end

@interface XRAnalysisCoreTableViewControllerResponse : NSObject
- (XRAnalysisCorePivotArray *)rows;
@end

@interface DTRenderableContentResponse : NSObject
- (XRAnalysisCoreTableViewControllerResponse *)content;
@end

@interface XRAnalysisCoreTableViewController : NSViewController <XRFilteredDataSource, XRSearchTarget>
- (DTRenderableContentResponse *)_currentResponse;
- (void)setDocumentInspectionTime:(XRTime)inspectionTime;
@end

@interface XRManagedEventArrayController : NSArrayController
@end

@interface XRLegacyInstrument : XRInstrument <XRInstrumentViewController, XRContextContainer>
- (NSArray<XRContext *> *)_permittedContexts;
@end

@interface XRRawBacktrace : NSObject
@end

@interface XRManagedEvent : NSObject
- (UInt32)identifier;
@end

@interface XRObjectAllocEvent : XRManagedEvent
- (UInt32)allocationEvent;
- (UInt32)destructionEvent;
- (UInt32)pastEvent;
- (UInt32)futureEvent;
- (BOOL)isAliveThroughIdentifier:(UInt32)identifier;
- (NSString *)eventTypeName;
- (NSString *)categoryName;
- (XRTime)timestamp; // Time elapsed from the beginning of the run.
- (SInt32)size; // in bytes
- (SInt32)delta; // in bytes
- (UInt64)address;
- (UInt64)slot;
- (UInt64)data;
- (XRRawBacktrace *)backtrace;
@end

@interface XRObjectAllocEventViewController : NSObject {
    XRManagedEventArrayController *_ac;
}
@end

@interface XROAArrayController : NSArrayController
{
    XRInstrument *_instrument;
}
@end


@interface XRObjectAllocInstrument : XRLegacyInstrument {
    XRObjectAllocEventViewController *_objectListController;
    XROAArrayController *_summaryController;
}
- (NSArray<XRContext *> *)_topLevelContexts;
- (NSArray<XRContext *> *)_summaryView;

- (void)setInspectionTime:(unsigned long long)arg1;
- (void)setSelectedTimeRange:(XRTimeRange)arg1;
@end

// MARK: - Memory leaks

@interface XRLeaksRun : XRRun
- (NSArray *)allLeaks;
@end

@interface DVT_VMUClassInfo : NSObject
- (NSString *)remoteClassName;
- (NSString *)genericInfo;
- (UInt32)instanceSize;
@end

@interface XRLeak : NSObject
- (NSString *) name;
- (unsigned long) size;
- (unsigned long) count;
- (BOOL) inCycle;
- (BOOL) isRootLeak;
- (unsigned long long) allocationTimestamp;
- (NSString *) displayAddress;
- (DVT_VMUClassInfo *) classInfo;
- (DVT_VMUClassInfo *) _layout;
@end

@interface XROAEventSummary : NSObject <NSCoding, NSCopying>
{
    long long totalBytes;
    long long activeBytes;
    int totalAllocationCount;
    int activeAllocationCount;
    int totalEvents;
    int livingCount;
    int transitoryCount;
    int categoryIdentifier;
    long long livingBytes;
    long long transitoryBytes;
    NSString *categoryName;
}
@end
