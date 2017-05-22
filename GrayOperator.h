#pragma once

void LineTrans(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height, UINT rowlen, UINT channel, double slope, double inter);

void LogTrans(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height, UINT rowlen, UINT channel, double a, double b);

void PowerTrans(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height, UINT rowlen, UINT channel, double b, double c, double r);

void ExpTrans(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height, UINT rowlen, UINT channel, double a, double b, double c);

void GrayStretch(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height, UINT rowlen, UINT channel, float X1, float Y1, float X2, float Y2);

void ThresholdTrans(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height, UINT rowlen, UINT channel, int thr);

void GrayEqualize(BYTE* pixel, BYTE* tempPixel, UINT width, UINT height);
