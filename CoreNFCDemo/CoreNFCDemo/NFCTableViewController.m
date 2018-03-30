//
//  NFCTableViewController.m
//  CoreNFCDemo
//
//  Created by Sands_Lee on 2018/3/29.
//  Copyright © 2018年 Sands_Lee. All rights reserved.
//

#import "NFCTableViewController.h"
#import "NFCTableViewCell.h"
#import <CoreNFC/CoreNFC.h>

@interface NFCTableViewController ()<NFCNDEFReaderSessionDelegate>

/**
 NFC Session
 */
@property (nonatomic, strong) NFCNDEFReaderSession *nfcSession;

/**
 founded NFC Messages
 */
@property (nonatomic, strong) NSMutableArray<NSArray<NFCNDEFMessage *> *> *nfcMessages;

@end

@implementation NFCTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // 注册tableViewCell
    [self.tableView registerClass:[NFCTableViewCell class] forCellReuseIdentifier:@"NFCTableCell"];
    // 初始化NFC
    [self initializeNFCSession];
}

#pragma mark - initializeNFC

- (void)initializeNFCSession {
    self.nfcSession = [[NFCNDEFReaderSession alloc] initWithDelegate:self queue:dispatch_get_main_queue() invalidateAfterFirstRead:NO];
    self.nfcSession.alertMessage = @"You can scan NFC-tags by holding them behind the top of your iPhone.";
}


#pragma mark - Action

- (IBAction)startSearchBtnClick:(UIBarButtonItem *)sender {
    NSLog(@"%s", __func__);
    
    [self.nfcSession beginSession];
}



#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return self.nfcMessages.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.nfcMessages[section].count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    NFCTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"NFCTableCell" forIndexPath:indexPath];
    NFCNDEFMessage *nfcMessage = self.nfcMessages[indexPath.section][indexPath.row];
    cell.textLabel.text = [NSString stringWithFormat:@"%zd Records", nfcMessage.records.count];
    
    return cell;
}


#pragma mark - Table view delegate

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    NSUInteger count = self.nfcMessages[section].count;
    NSString *title = count == 1 ? @"One Message" : [NSString stringWithFormat:@"%zd Messages", count];
    return title;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NFCNDEFMessage *message = self.nfcMessages[indexPath.section][indexPath.row];
    NSMutableArray<NSString *> *payloadStrs = [NSMutableArray array];
    for (NFCNDEFPayload * record in message.records) {
        [payloadStrs addObject:[[NSString alloc] initWithData:record.payload encoding:NSUTF8StringEncoding]];
    }
    NSString *alertTitle = [NSString stringWithFormat:@"%zd Records found in Message.", message.records.count];
    NSString *alertMsg = [payloadStrs componentsJoinedByString:@"\n"];
    UIAlertView *alertV = [[UIAlertView alloc] initWithTitle:alertTitle
                                                     message:alertMsg
                                                    delegate:nil
                                           cancelButtonTitle:@"OK"
                                           otherButtonTitles:nil, nil];
    [alertV show];
    
    [self.tableView deselectRowAtIndexPath:indexPath animated:YES];
}


#pragma mark - NFCNDEFReaderSessionDelegate

/*!
 * @method readerSession:didInvalidateWithError:
 *
 * @param session   The session object that is invalidated.
 * @param error     The error indicates the invalidation reason.
 *
 * @discussion      Gets called when a session becomes invalid.  At this point the client is expected to discard
 *                  the returned session object.
 */

/**
 NFC 读取的session发生错误或session过期时回调，此时客户端应当丢弃返回的session，即此session不可重用。

 @param session 发生错误的session
 @param error 错误信息
 */
- (void)readerSession:(NFCNDEFReaderSession *)session didInvalidateWithError:(NSError *)error {
    if (error)
    {
        NSLog(@"NFC-Session invalidated: %@", error);
    }
    if (self.nfcSession)
    {
        // 关闭当前session
        [self.nfcSession invalidateSession];
        self.nfcSession = nil;
    }
    // 重新初始化一个新的session
    [self initializeNFCSession];
}


/*!
 * @method readerSession:didDetectNDEFs:
 *
 * @param session   The session object used for tag detection.
 * @param messages  Array of @link NFCNDEFMessage @link/ objects. The order of the discovery on the tag is maintained.
 *
 * @discussion      Gets called when the reader detects NFC tag(s) with NDEF messages in the polling sequence.  Polling
 *                  is automatically restarted once the detected tag is removed from the reader's read range.
 */

/**
 当NFC Session在轮询队列中读取到NDEF信息时回调，此时轮询会自动重启一次再次检测NFC标签是否离开了读取范围。（原理类似于机械按键的防抖动）

 @param session 读取Session
 @param messages 读取到的信息
 */
- (void)readerSession:(NFCNDEFReaderSession *)session didDetectNDEFs:(NSArray<NFCNDEFMessage *> *)messages {
    NSLog(@"New NFC Messages %zd detected:", messages.count);
    for (NFCNDEFMessage *message in messages) {
        NSLog(@"- %zd Records:", message.records.count);
        for (NFCNDEFPayload *record in message.records) {
            NSLog(@"\t- TNF(TypeNameFormat): %@", [self formattedTypeNameFormat:record.typeNameFormat]);
            NSLog(@"\t- Payload: %@", [[NSString alloc] initWithData:record.payload encoding:NSUTF8StringEncoding]);
            NSLog(@"\t- Type: %@", [[NSString alloc] initWithData:record.type encoding:NSUTF8StringEncoding]);
            NSLog(@"\t- Identifier: %@", [[NSString alloc] initWithData:record.identifier encoding:NSUTF8StringEncoding]);
        }
    }
    
    [self.nfcMessages addObject:messages];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

#pragma mark - NFCReaderSessionDelegate

/*!
 * @method readerSessionDidBecomeActive:
 *
 * @param session   The session object in the active state.
 *
 * @discussion      Gets called when the NFC reader session has become active. RF is enabled and reader is scanning for tags.
 *                  The @link readerSession:didDetectTags: @link/ will be called when a tag is detected.
 */

/**
 NFC 读取 Session激活时调用

 @param session NFC 读取 Session
 */
- (void)readerSessionDidBecomeActive:(NFCReaderSession *)session {
    NSLog(@"%s", __func__);
}


/*!
 * @method readerSession:didDetectTags:
 *
 * @param session   The session object used for tag detection.
 * @param tags      Array of @link NFCTag @link/ objects.
 *
 * @discussion      Gets called when the reader detects NFC tag(s) in the polling sequence.
 */

/**
 检测到 NFC 标签时调用

 @param session NFC 读取 Session
 @param tags 标签
 */
- (void)readerSession:(NFCReaderSession *)session didDetectTags:(NSArray<__kindof id<NFCTag>> *)tags {
    NSLog(@"%s", __func__);
    NSLog(@"- detected tags: %@", tags);
}

#pragma mark - getter

- (NSMutableArray<NSArray<NFCNDEFMessage *> *> *)nfcMessages {
    if (!_nfcMessages) {
        _nfcMessages = [NSMutableArray arrayWithCapacity:5];
    }
    return _nfcMessages;
}

- (NSString *)formattedTypeNameFormat:(NFCTypeNameFormat)typeNameFormat {
    switch (typeNameFormat) {
        case NFCTypeNameFormatEmpty:
            return @"Empty";
            break;
        case NFCTypeNameFormatNFCWellKnown:
            return @"NFC Well Known";
            break;
        case NFCTypeNameFormatMedia:
            return @"Media";
            break;
        case NFCTypeNameFormatAbsoluteURI:
            return @"Absolute URI";
            break;
        case NFCTypeNameFormatNFCExternal:
            return @"NFC External";
            break;
        case NFCTypeNameFormatUnknown:
            return @"Unknown";
            break;
        case NFCTypeNameFormatUnchanged:
            return @"Unchanged";
            break;
            
        default:
            return @"Format Unknown";
            break;
    }
}

@end
