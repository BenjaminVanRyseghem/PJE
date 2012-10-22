/*
 * tp5.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: benjamin
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv.hpp>
#include <cvblob.h>
#include <highgui/highgui_c.h>
#include <cvblobs/BlobResult.h>

using namespace std;

IplImage* createImage(){
	CvScalar WHITE = CV_RGB( 255, 255, 255 );
	IplImage* imBlobs = cvCreateImage( cvSize( 400, 300 ), IPL_DEPTH_8U, 1 );
	cvZero( imBlobs );
	cvRectangle( imBlobs, cvPoint( 100, 100 ), cvPoint( 200, 150 ), WHITE, -1 );
	// Žpaisseur -1 : forme pleine
	cvCircle( imBlobs, cvPoint( 200, 200 ), 30, WHITE, -1 );
	cvLine( imBlobs, cvPoint( 300, 200 ), cvPoint( 300, 250 ), WHITE );
	return imBlobs;
}


/* --------------------------------------------------------------------------
Programme principal
-------------------------------------------------------------------------- */
int main () {
	IplImage* cvImage = 0;
	CvCapture * camera = cvCaptureFromCAM( 0 );
    // Definir les variables locales

	CvScalar colors[100];
	srand ( time (NULL) );
	for (int i = 0; i<100; i++)
	{
	    CvScalar newColor;
	    bool reallyNew;
	    do {
	        newColor = CV_RGB( rand()&255, rand()&255, rand()&255 );
	        int j = -1; reallyNew = true;
	        do {
	            j++;
	        } while ( (colors[j].val[0] != newColor.val[0] || colors[j].val[1] != newColor.val[1]
	        || colors[j].val[2] != newColor.val[2]) && j < i);
	        if ( (colors[j].val[0] == newColor.val[0] && colors[j].val[1] == newColor.val[1]
	        && colors[j].val[2] == newColor.val[2]) )
	        reallyNew = false;
	    } while (!reallyNew);
	    colors[i] = newColor;
	}


	// Creer la fentre d'affichage de l'image
	cvNamedWindow ("Acquisition", CV_WINDOW_AUTOSIZE);
	cvNamedWindow ("Modified", CV_WINDOW_AUTOSIZE);
	// Boucler pour afficher les images
	int iKey = 0;
	while (iKey != 27) {
		// AcquŽrir l'image
		int size;

		if(!cvGrabFrame(camera)){              // capture a frame
		  printf("Could not grab a frame\n\7");
		  exit(0);
		}
		cvImage = cvRetrieveFrame(camera);
		cvFlip(cvImage, cvImage, 1);
		IplImage *im_gray = cvCreateImage(cvGetSize(cvImage),IPL_DEPTH_8U,1);
		cvCvtColor(cvImage,im_gray,CV_RGB2GRAY);

		cvThreshold(im_gray, im_gray, 220, 255, CV_THRESH_BINARY);


		CBlobResult result = CBlobResult::CBlobResult(im_gray, im_gray, false);
		result.Filter(result, B_EXCLUDE, CBlobGetArea(),B_LESS,50.0, 0.0);
		result.Filter(result, B_EXCLUDE, CBlobGetDiffX(),B_LESS,50.0, 0.0);
		result.Filter(result, B_EXCLUDE, CBlobGetDiffY(),B_LESS,50.0, 0.0);
		result.Filter(result, B_EXCLUDE, CBlobGetArea(),B_GREATER,6000.0, 0.0);
		result.Filter(result, B_EXCLUDE, CBlobGetPerimeter(),B_OUTSIDE,0.0, 1500.0);
		//result.Filter(result, B_EXCLUDE, CBlobGetAxisRatio(),B_LESS,0.6, 0.0);

		size = result.GetNumBlobs();
		cout << size << " blobs trouves" <<endl;
		for (int i = 0 ; i < size ; i ++){
			CBlob blob = result.GetBlob(i);
			cout << blob.GetID() << endl << blob.Area() << endl << blob.Perimeter() << endl;
			cout << "            ------           " << endl;
			/*CvBox2D ellipse = blob.GetEllipse();
			cout << "(" << ellipse.center.x << "," << ellipse.center.y << ")" << endl;
			cout << "(" << ellipse.size.height << "," << ellipse.size.width << ")" << endl;
			cout << "=============================" << endl;
			cvEllipse(
					cvImage,
						cvPointFrom32f(ellipse.center),
						cvSize( (int)ellipse.size.width, (int)ellipse.size.height ),
						ellipse.angle,
						0, 360,
						CV_RGB( 255, 0, 0 ));*/
			CvRect box = blob.GetBoundingBox();
			int id = blob.GetID();
			blob.FillBlob(cvImage,colors[id],0,0);
			cvRectangle(cvImage, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height), cvScalar(255,0,0), 1);
			CvPoint box_center = cvPoint(box.x+box.width/2,box.y+box.height/2);
			cvLine(cvImage,box_center,box_center,cvScalar(0,255,255),10,8,0);
			CvFont font;
			cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5 );
			char id_string[33];
			sprintf(id_string, "%d", id);

			cout << "ID = " << id_string << endl;

			cvPutText(cvImage, id_string, box_center, &font,cvScalar(0,255,255));
		}


		// Afficher l'image
		cvShowImage ("Acquisition", cvImage);
		cvShowImage ("Modified", im_gray);

		result.ClearBlobs();
		// Attendre 20 ms que l'utilisateur appuie sur une touche
		iKey = cvWaitKey (20);
	}
	cvReleaseCapture(&camera);
	// Liberer la memoire et finir sans erreur
	cvDestroyWindow ("Acquisition");
	cvDestroyWindow ("Modified");
	return (0);

}

/*
int main(){
	int size;

	IplImage* image = createImage();
	CBlobResult result = CBlobResult::CBlobResult(image, image, 0);
	result.Filter(result, B_EXCLUDE, CBlobGetArea(),B_LESS,1000.0, 0.0);
	result.Filter(result, B_EXCLUDE, CBlobGetAxisRatio(),B_LESS,0.6, 0.0);

	size = result.GetNumBlobs();
	for (int i = 0 ; i < size ; i ++){
		CBlob blob = result.GetBlob(i);
		cout << blob.GetID() << endl << blob.Area() << endl << blob.Perimeter() << endl;
		cout << "            ------           " << endl;
		CvBox2D ellipse = blob.GetEllipse();
		cout << "(" << ellipse.center.x << "," << ellipse.center.y << ")" << endl;
		cout << "(" << ellipse.size.height << "," << ellipse.size.width << ")" << endl;
		cout << "=============================" << endl;
		cvEllipse(
					image,
					cvPointFrom32f(ellipse.center),
					cvSize( (int)ellipse.size.width, (int)ellipse.size.height ),
					ellipse.angle,
					0, 360,
					CV_RGB( 127, 127, 127 ));
	}
	cvNamedWindow( "Foo" );
	cvShowImage( "Foo", image );


	result.ClearBlobs();
	cvWaitKey( 0 );
	cvDestroyWindow( "Foo" );
	cvReleaseImage( &image );
	return (0);
}
*/

