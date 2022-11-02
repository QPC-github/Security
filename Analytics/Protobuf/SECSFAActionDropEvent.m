// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from stdin

#import "SECSFAActionDropEvent.h"
#import <ProtocolBuffer/PBConstants.h>
#import <ProtocolBuffer/PBHashUtil.h>
#import <ProtocolBuffer/PBDataReader.h>

#if !__has_feature(objc_arc)
# error This generated file depends on ARC but it is not enabled; turn on ARC, or use 'objc_use_arc' option to generate non-ARC code.
#endif

@implementation SECSFAActionDropEvent

@synthesize excludeEvent = _excludeEvent;
- (void)setExcludeEvent:(BOOL)v
{
    _has.excludeEvent = YES;
    _excludeEvent = v;
}
- (void)setHasExcludeEvent:(BOOL)f
{
    _has.excludeEvent = f;
}
- (BOOL)hasExcludeEvent
{
    return _has.excludeEvent != 0;
}
@synthesize excludeCount = _excludeCount;
- (void)setExcludeCount:(BOOL)v
{
    _has.excludeCount = YES;
    _excludeCount = v;
}
- (void)setHasExcludeCount:(BOOL)f
{
    _has.excludeCount = f;
}
- (BOOL)hasExcludeCount
{
    return _has.excludeCount != 0;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%@ %@", [super description], [self dictionaryRepresentation]];
}

- (NSDictionary *)dictionaryRepresentation
{
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (self->_has.excludeEvent)
    {
        [dict setObject:[NSNumber numberWithBool:self->_excludeEvent] forKey:@"excludeEvent"];
    }
    if (self->_has.excludeCount)
    {
        [dict setObject:[NSNumber numberWithBool:self->_excludeCount] forKey:@"excludeCount"];
    }
    return dict;
}

BOOL SECSFAActionDropEventReadFrom(__unsafe_unretained SECSFAActionDropEvent *self, __unsafe_unretained PBDataReader *reader) {
    while (PBReaderHasMoreData(reader)) {
        uint32_t tag = 0;
        uint8_t aType = 0;

        PBReaderReadTag32AndType(reader, &tag, &aType);

        if (PBReaderHasError(reader))
            break;

        if (aType == TYPE_END_GROUP) {
            break;
        }

        switch (tag) {

            case 1 /* excludeEvent */:
            {
                self->_has.excludeEvent = YES;
                self->_excludeEvent = PBReaderReadBOOL(reader);
            }
            break;
            case 2 /* excludeCount */:
            {
                self->_has.excludeCount = YES;
                self->_excludeCount = PBReaderReadBOOL(reader);
            }
            break;
            default:
                if (!PBReaderSkipValueWithTag(reader, tag, aType))
                    return NO;
                break;
        }
    }
    return !PBReaderHasError(reader);
}

- (BOOL)readFrom:(PBDataReader *)reader
{
    return SECSFAActionDropEventReadFrom(self, reader);
}
- (void)writeTo:(PBDataWriter *)writer
{
    /* excludeEvent */
    {
        if (self->_has.excludeEvent)
        {
            PBDataWriterWriteBOOLField(writer, self->_excludeEvent, 1);
        }
    }
    /* excludeCount */
    {
        if (self->_has.excludeCount)
        {
            PBDataWriterWriteBOOLField(writer, self->_excludeCount, 2);
        }
    }
}

- (void)copyTo:(SECSFAActionDropEvent *)other
{
    if (self->_has.excludeEvent)
    {
        other->_excludeEvent = _excludeEvent;
        other->_has.excludeEvent = YES;
    }
    if (self->_has.excludeCount)
    {
        other->_excludeCount = _excludeCount;
        other->_has.excludeCount = YES;
    }
}

- (id)copyWithZone:(NSZone *)zone
{
    SECSFAActionDropEvent *copy = [[[self class] allocWithZone:zone] init];
    if (self->_has.excludeEvent)
    {
        copy->_excludeEvent = _excludeEvent;
        copy->_has.excludeEvent = YES;
    }
    if (self->_has.excludeCount)
    {
        copy->_excludeCount = _excludeCount;
        copy->_has.excludeCount = YES;
    }
    return copy;
}

- (BOOL)isEqual:(id)object
{
    SECSFAActionDropEvent *other = (SECSFAActionDropEvent *)object;
    return [other isMemberOfClass:[self class]]
    &&
    ((self->_has.excludeEvent && other->_has.excludeEvent && ((self->_excludeEvent && other->_excludeEvent) || (!self->_excludeEvent && !other->_excludeEvent))) || (!self->_has.excludeEvent && !other->_has.excludeEvent))
    &&
    ((self->_has.excludeCount && other->_has.excludeCount && ((self->_excludeCount && other->_excludeCount) || (!self->_excludeCount && !other->_excludeCount))) || (!self->_has.excludeCount && !other->_has.excludeCount))
    ;
}

- (NSUInteger)hash
{
    return 0
    ^
    (self->_has.excludeEvent ? PBHashInt((NSUInteger)self->_excludeEvent) : 0)
    ^
    (self->_has.excludeCount ? PBHashInt((NSUInteger)self->_excludeCount) : 0)
    ;
}

- (void)mergeFrom:(SECSFAActionDropEvent *)other
{
    if (other->_has.excludeEvent)
    {
        self->_excludeEvent = other->_excludeEvent;
        self->_has.excludeEvent = YES;
    }
    if (other->_has.excludeCount)
    {
        self->_excludeCount = other->_excludeCount;
        self->_has.excludeCount = YES;
    }
}

@end
