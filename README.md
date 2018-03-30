# iOS 11 迟来的 NFC 

## 前言

NFC这个词相信大家现在都已经不陌生了，各大城市的地铁、商场等等支持NFC支付一度成为头条的热点。其实很早之前就已经有二维码和NFC的诞生了，但是由于二维码成本低廉，技术门槛相对较低，因此，二维码迅速抢占了移动支付的市场，但是，与此同时NFC的发展并未因此停止。其实在安卓端的NFC发展已经非常迅猛了，只是我们的苹果爸爸迟迟不肯带我们一起玩NFC。终于，在去年的WWDC上，苹果宣布**~~“开放”~~**其NFC接口，这为以后NFC的应用开发提供了更多的可能。

## 关于NFC

NFC（Near Field Communication）近场通信，当两个设备相互靠近时能进行信息交流的一个技术。使用了NFC技术的设备（比如手机）可以在彼此靠近的情况下进行数据交换，是由非接触式射频识别(RFID)及互连互通技术整合演变而来，通过在单一芯片上集成感应式读卡器、感应式卡片和点对点通信的功能，利用移动终端实现移动支付、电子票务、门禁、移动身份识别、防伪等应用。目前，苹果的CoreNFC对NFC的格式支持有限，暂时仅支持NDEF格式。

## 关于NDEF

NDEF（NFC Data Exchange Format）是一种能够在NFC设备或者标签之间进行信息交换的数据格式。NDEF格式由各种 `NDEF Messages` 和 `NDEF Records` 组成，NDEF格式使用一种容易理解的格式，存储和交换信息如：uri、纯文本等等。NFC标签像`Mifare Classic`卡片可以配置为NDEF标签,通过一个NFC设备和数据写入(NDEF记录)可以被其他NDEF兼容的设备访问。NDEF消息还可以用于两个活跃的NFC设备之间“点对点”模式交换数据。

![NDEF格式](http://ojca2gwha.bkt.clouddn.com/CoreNFC-tags.png)

#### NDEF Messages

`NDEF Messages`是`NDEF Records`交换机制的基础，每一个message包含一个或多个records。

#### NDEF Records

`NDEF Records`包含一个特定的`payload`，并且有以下结构来标识内容和记录大小:

![record](http://onmw6wg88.bkt.clouddn.com/Snip20180329_1.png)

#### Record Header（记录头）

记录头包含了很多重要的信息，它占用3个位来标识遵循`TNF`协议的记录的类型，讲人话就是这3个位用来表示这条记录的类型的。

##### TNF: Type Name Format 字段

一条NDEF记录的类型名称是一个3个位的数值，用来描述这条记录的类型，并且可以用来设置对该记录中其它的结构和内容的期望。简单的说就是这3个位不仅可以表示该条记录的类型，也可以在一定程度上决定了该条记录接下来的数据结构。可能的记录名称如下表：


| TNF Value | Record Type |
| --- | :-- |
| 0x00 | Empty Record 表明这条记录没有类型、id或有效payload。这个记录类型一般用于新格式化的NDEF卡上，因为NDEF标签必须有至少一个NDEF纪录。
 |
| 0x01 | Well-Known Record 表明记录类型字段使用RTD类型名称格式。这种类型名称用一个Record Type Definition (RTD)来存储任何指定的类型，例如：存储RTD文本、RTD URIs等等。同时，这是一种比较常用的也比较有用的记录类型。 |
| 0x02 | MIME Media Record 表明payload是这条NDEF记录分块的中间或者最后一块。 |
| 0x03 | Absolute URI Record 表明这条记录的类型字段一定包含一个URI字段。 |
| 0x04 | External Record 表明这条记录的类型字段包含一个RTD格式的外部字段。 |
| 0x05 | Unknown Record 表明payload的类型未知。 |
| 0x06 | Unchanged Record 未发生变化的记录类型，释同MIME Media Record。 |

##### IL：ID Length 字段

`IL`是ID长度的标志位，用来表示下面的`ID Length`字段是否省略。如果这个位设置为0，则该条记录中省略`ID Length`。

##### SR：Short Record 位

短记录标志位，如果下面的`PAYLOAD LENGTH`字段小于等于一个字节，则该位设置为1 。

##### CF：Chunk Flag

块标识位，用于标识当前块是第一个记录块还是中间的记录块。

##### ME：Message End

结束标志位，用于标识当前记录是否是当前Message的最后一条记录。

##### MB：Message Begin

起始标志位，用于标识当前记录是否是当前Message的第一条记录。

#### Type Length

表示类型字段的长度。对于上面`TNF`字段描述中的某些值，该字段一直为0 。

#### Payload Length

表示该条记录的payload字段长度。如果上面的`SR`字段设置为1，则该字段占用1个字节的长度，但是如果`SR`设置为0，则该字段将有32个位，占用4个字节的长度。

#### ID Length

表示该条记录的ID的长度。只有当上面的`IL`位置1时该字段才会被省略。

#### Record Type

表示记录的类型，这个字段的值必须根据`TNF`位的设置确定。

#### Record ID

表示该条记录的ID。当`IL`位为0时，该字段省略。

#### Payload

表示该条记录的payload，该字段的长度务必与上面的`Payload Length`字段值一致。

## iOS CoreNFC

下面将通过一个简单的示例来演示怎么使用苹果爸爸推出的CoreNFC，该示例仅可以用来读取存储在卡片上的NDEF格式的信息。

> 为此，我使用了`STM32F407`单片机与`Adafruit PN532 Shield`NFC读写模块配对，将信息写入NDEF格式的卡片上。本文中，我不会记录如何将普通的`Mifare Classic`卡片格式化成NDEF格式的卡片以及如何将数据写入到NDEF卡片中（iOS的CoreNFC暂时不支持写入）。

我们的app要使用NFC必须要进行应用授权：首先要创建一个支持NFC的证书，并且开启`NFC Tag Reading`，如下图：

![nfccer](http://onmw6wg88.bkt.clouddn.com/nfccer.png)

导入证书之后，我们需要进行Info.plist配置`Privacy - NFC Scan Usage Description`权限，如下图：

![Snip20180329_3](http://onmw6wg88.bkt.clouddn.com/Snip20180329_3.png)

要实现NFC功能，我们得先导入`CoreNFC.framework`，并导入其头文件`#import <CoreNFC/CoreNFC.h>`目前为止，iOS模拟器还不支持CoreNFC，只能使用真机调试。

#### 1. 初始化Session

与二维码扫描等类似，NFC 也具备一个用于信息交互的Session，并且这个Session要在使用期间一直持有，所以初始化Session代码如下：

```objc
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

#pragma mark - initializeNFC

- (void)initializeNFCSession {
    // 创建 Session
    self.nfcSession = [[NFCNDEFReaderSession alloc] initWithDelegate:self queue:dispatch_get_main_queue() invalidateAfterFirstRead:NO];
    // 设置 Session 的提示信息
    self.nfcSession.alertMessage = @"You can scan NFC-tags by holding them behind the top of your iPhone.";
}

@end
```


#### 2. 启动Session

经过上面的初始化之后，有了Session就可以开启监听了，启动监听很简单，示例代码如下：

```objc
- (IBAction)startSearchBtnClick:(UIBarButtonItem *)sender {
    NSLog(@"%s", __func__);
    // 启动Session
    [self.nfcSession beginSession];
}
```

Session启动时会自动调出系统的扫描面板，如下图：


#### 3. 监听代理回调

通过上面的方式把Session启动之后，设备就会自动开始扫描NDEF格式的标签信息，当扫描到标签信息，或者发生任何异常时都会通过代理方法回调，所以我们需要监听Session的回调信息，示例如下：

```objc
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
```





