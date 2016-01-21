//#include <opencv2/highgui.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/objdetect.hpp>
//
//
//using namespace std;
//using namespace cv;
//
//void doMosaic(IplImage* in, int x0, int y0,
//	int width, int height, int size)
//{
//	int b, g, r, col, row;
//
//	int xMin = size*(int)floor((double)x0 / size);
//	int yMin = size*(int)floor((double)y0 / size);
//	int xMax = size*(int)ceil((double)(x0 + width) / size);
//	int yMax = size*(int)ceil((double)(y0 + height) / size);
//
//	for (int y = yMin; y < yMax; y += size){
//		for (int x = xMin; x < xMax; x += size){
//			b = g = r = 0;
//			for (int i = 0; i<size; i++){
//				if (y + i > in->height){
//					break;
//				}
//				row = i;
//				for (int j = 0; j<size; j++){
//					if (x + j > in->width){
//						break;
//					}
//					b += (unsigned char)in->imageData[in->widthStep*(y + i) + (x + j) * 3];
//					g += (unsigned char)in->imageData[in->widthStep*(y + i) + (x + j) * 3 + 1];
//					r += (unsigned char)in->imageData[in->widthStep*(y + i) + (x + j) * 3 + 2];
//					col = j;
//				}
//			}
//			row++;
//			col++;
//			for (int i = 0; i < row; i++){
//				for (int j = 0; j < col; j++){
//					in->imageData[in->widthStep*(y + i) + (x + j) * 3] = cvRound((double)b / (row*col));
//					in->imageData[in->widthStep*(y + i) + (x + j) * 3 + 1] = cvRound((double)g / (row*col));
//					in->imageData[in->widthStep*(y + i) + (x + j) * 3 + 2] = cvRound((double)r / (row*col));
//				}
//			}
//		}
//	}
//}
//
//int main(int argc, char **argv)
//{
//
//	VideoCapture v(0);
//	namedWindow("l");
//	if (!v.isOpened())return 2;
//	IplImage *src_gray = 0;
//	const char *cascade_name = "C:/Users/e.yilmaz/Downloads/haarcascade_frontalface_default.xml";
//	CvHaarClassifierCascade *cascade = 0;
//	CvMemStorage *storage = 0;
//	CvSeq *faces;
//
//	cascade = (CvHaarClassifierCascade *)cvLoad(cascade_name, 0, 0, 0);
//
//	while (1)
//	{
//		Mat f;
//		v.read(f);
//		imshow("l", f);
//		IplImage copy = f;
//		IplImage* src_img = &copy;
//		src_gray = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
//
//		storage = cvCreateMemStorage(0);
//		cvClearMemStorage(storage);
//
//		cvCvtColor(src_img, src_gray, CV_BGR2GRAY);
//		cvEqualizeHist(src_gray, src_gray);
//	
//		faces = cvHaarDetectObjects(src_gray, cascade, storage,
//			1.11, 4, 0, cvSize(40, 40));
//		for (int i = 0; i < (faces ? faces->total : 0); i++) {
//			CvRect *r = (CvRect *)cvGetSeqElem(faces, i);
//			doMosaic(src_img, r->x, r->y, r->width, r->height, 20);
//
//		}
//
//		cvShowImage("Capture", src_img);
//		cvReleaseImage(&src_gray);
//
//		int c = cvWaitKey(2);
//		if (c == '\x1b')
//			break;
//
//		if (char c = waitKey(10) == 27) break;
//	}
//	cvDestroyWindow("l");
//
//	return 0;
//}
//
