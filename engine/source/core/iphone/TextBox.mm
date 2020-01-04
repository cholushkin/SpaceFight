//
//  TextBox.m
//  iPhone
//
//  Created by Admin on 08.12.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "wchar.h"

@interface TextBoxAlert : NSObject<UIAlertViewDelegate> {
  NSInteger clickedButtonIndex;
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;

@property (nonatomic, assign) NSInteger clickedButtonIndex;

@end

@implementation TextBoxAlert

@synthesize clickedButtonIndex;

// Called when a button is clicked. The view will be automatically dismissed after this call returns
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
  clickedButtonIndex = buttonIndex;
}

@end

@interface NSString (WideCharOps)
- (wchar_t *)getWideChars;
+ (NSString *)stringWithWideChars:(const wchar_t *)src;
@end

@implementation NSString (WideCharOps)

- (wchar_t *)getWideChars
{
  unsigned long len = [self length];
  wchar_t *buffer = new wchar_t[len+1];
  [self getCString:(char*)buffer maxLength:(len+1)*sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding];
  buffer[len] = L'\0';
  return buffer;
}

+ (NSString *)stringWithWideChars:(const wchar_t *)src
{
  NSString *n = [[NSString alloc]initWithBytes:(const char*)src length: wcslen(src) * sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding];
  return n;
}
@end  

extern "C" bool TextBox(const wchar_t * const szTitle, wchar_t* buffer, unsigned int size) {
  //
  TextBoxAlert* textbox = [[TextBoxAlert alloc] init];
  textbox.clickedButtonIndex = -1;
  UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[NSString stringWithWideChars:szTitle]
                                                  message:@"\n"
                                                 delegate:textbox cancelButtonTitle:@"Dismiss" otherButtonTitles:@"OK", nil];
  UITextField *myTextField = [[UITextField alloc] initWithFrame:CGRectMake(12, 45, 260, 25)];

  NSString *reqSysVer = @"4.0";
  NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
  if ([currSysVer compare:reqSysVer options:NSNumericSearch] == NSOrderedAscending)
  {
    CGAffineTransform myTransform = CGAffineTransformMakeTranslation(0, 130);
    [alert setTransform:myTransform];
  }

  [myTextField setBackgroundColor:[UIColor whiteColor]];
  if(buffer[0])
    myTextField.text = [NSString stringWithWideChars:buffer];
  [alert addSubview:myTextField];
  [myTextField becomeFirstResponder];
  [alert show];
  while (textbox.clickedButtonIndex == -1) {
    [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.25]];
  }

  bool res = alert.cancelButtonIndex != textbox.clickedButtonIndex;
  if(res)
  {
    [myTextField.text getCString:(char*)buffer maxLength:size * sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding];
    buffer[size - 1] = NULL;
  }
  return res;
}