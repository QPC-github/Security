// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from SecDbKeychainSerializedMetadataKey.proto

#import <Foundation/Foundation.h>
#import <ProtocolBuffer/PBCodable.h>

#ifdef __cplusplus
#define SECDBKEYCHAINSERIALIZEDMETADATAKEY_FUNCTION extern "C"
#else
#define SECDBKEYCHAINSERIALIZEDMETADATAKEY_FUNCTION extern
#endif

@interface SecDbKeychainSerializedMetadataKey : PBCodable <NSCopying>
{
    int32_t _actualKeyclass;
    NSData *_akswrappedkey;
    NSData *_backupwrappedkey;
    NSData *_baguuid;
    int32_t _keyclass;
    struct {
        int actualKeyclass:1;
        int keyclass:1;
    } _has;
}


@property (nonatomic) BOOL hasKeyclass;
@property (nonatomic) int32_t keyclass;

@property (nonatomic) BOOL hasActualKeyclass;
@property (nonatomic) int32_t actualKeyclass;

@property (nonatomic, readonly) BOOL hasBaguuid;
@property (nonatomic, retain) NSData *baguuid;

@property (nonatomic, readonly) BOOL hasAkswrappedkey;
@property (nonatomic, retain) NSData *akswrappedkey;

@property (nonatomic, readonly) BOOL hasBackupwrappedkey;
@property (nonatomic, retain) NSData *backupwrappedkey;

// Performs a shallow copy into other
- (void)copyTo:(SecDbKeychainSerializedMetadataKey *)other;

// Performs a deep merge from other into self
// If set in other, singular values in self are replaced in self
// Singular composite values are recursively merged
// Repeated values from other are appended to repeated values in self
- (void)mergeFrom:(SecDbKeychainSerializedMetadataKey *)other;

SECDBKEYCHAINSERIALIZEDMETADATAKEY_FUNCTION BOOL SecDbKeychainSerializedMetadataKeyReadFrom(__unsafe_unretained SecDbKeychainSerializedMetadataKey *self, __unsafe_unretained PBDataReader *reader);

@end
