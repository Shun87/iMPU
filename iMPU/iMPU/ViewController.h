//
//  ViewController.h
//  PreviewController
//
//  Created by  on 12-3-23.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GLDisplay/GLView.h"
#import "VideoCapturer.h"

#import <MessageUI/MessageUI.h>
@interface ViewController : UIViewController< MFMailComposeViewControllerDelegate, UINavigationControllerDelegate>
{
//    VideoCapturer *processor;
//    GLView *glView;
//    UIView *backView;
    
}
@property (nonatomic, retain) IBOutlet UIView *backView;

- (IBAction)btnSend:(id)sender;
@end
