//this code was made with mingw and 3.0 opencv version
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>

#include <opencv/cvaux.hpp>//
#include <opencv/cxcore.hpp>//
#include <stdlib.h>

#include <opencv2/opencv.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
#include <string>

#include <Windows.h>
 //esta libreria es para Sleep

using namespace cv;//this is just to avoid write 'cv::Point(zoneAXR...'
                   //you can just write 'Point(zoneAXR...'
using namespace std;

// Global variables
Mat frame; //current frame
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Mat gray;
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
int keyboard; //input from keyboard
int percentX = 65;
int percentY = 40;
int zoneAXL;int zoneAYU;int zoneAXR;int zoneAYD;
int zoneBXL;int zoneBYU;int zoneBXR;int zoneBYD;

int au = 0;//incialize zone A down counter to zero counts how many people has
           //crosed A zone up bound
int refXau = 0;//when a person enter on the detectable area beside A zone up bound
               //the value x is stored to be compared with the next frame near point
int refYau = 0;//when a person enter on the detectable area beside A zone up bound
               //the value y is stored to be compared with the next frame near point
bool sigueau = false;//turns true when a center of mass has crossed the bound
bool cercaau = false;//turns true when a point of mass is near to one earlier
bool waitau = true;//deactivates for some frames to dont make a false count of the same
                   //person turning false
int waitcontau = 0;//counts how many frames since waitau turned false usded on 'sigue'
                   //bucle to evaluate if sigue is inicialize
int cuentaau = 0;//counts how many frames since waitau turned false used on 'waitua'
                 //bucle to inicialize if a second point has not been found
int nuevoau;//contains the second value obtained of center of mass to compare to refYau
            //we only want to count if the person is walking on the desired direction

int al = 0;// incialize zone A left to zero
int refXal = 0;
int refYal = 0;
bool sigueal = false;
bool cercaal = false;
bool waital = true;//activates or deactivates
int waitcontal = 0;//counts how many frames since arx was activated
int cuentaal = 0;
int nuevoal;//contains the second value obtained of center of mass to compare to refXal

int ar = 0;// incialize zone A right to zero
int refXar = 0;
int refYar = 0;
bool siguear = false;
bool cercaar = false;
bool waitar = true;//activates or deactivates
int waitcontar = 0;//counts how many frames since arx was activated
int cuentaar = 0;
int nuevoar;//contains the second value obtained of center of mass to compare to refXar

int ad = 0;// incialize zone A down to zero
int refXad = 0;
int refYad = 0;
bool siguead = false;
bool cercaad = false;
bool waitad = true;//activates or deactivates
int waitcontad = 0;//counts how many frames since ady was activated (detected)
int cuentaad = 0;
int nuevoad;//contains the second value obtained of center of mass to compare to refYad

int bu = 0;// incialize zone A down to zero
int refXbu = 0;
int refYbu = 0;
bool siguebu = false;
bool cercabu = false;
bool waitbu = true;//activates or deactivates
int waitcontbu = 0;//counts how many frames since ady was activated (detected)
int cuentabu = 0;
int nuevobu;//contains the second value obtained of center of mass to compare

int bl = 0;// incialize zone B left to zero
int refXbl = 0;
int refYbl = 0;
bool siguebl = false;
bool cercabl = false;
bool waitbl = true;//activates or deactivates
int waitcontbl = 0;//counts how many frames since arx was activated
int cuentabl = 0;
int nuevobl;//contains the second value obtained of center of mass to compare

int br = 0;// incialize zone B right to zero
int refXbr = 0;
int refYbr = 0;
bool siguebr = false;
bool cercabr = false;
bool waitbr = true;//activates or deactivates
int waitcontbr = 0;//counts how many frames since arx was activated
int cuentabr = 0;
int nuevobr;//contains the second value obtained of center of mass to compare

int bd = 0;// incialize zone A down to zero
int refXbd = 0;
int refYbd = 0;
bool siguebd = false;
bool cercabd = false;
bool waitbd = true;//activates or deactivates
int waitcontbd = 0;//counts how many frames since ady was activated (detected)
int cuentabd = 0;
int nuevobd;//contains the second value obtained of center of mass to compare

int const waitvalue = 35;//how big must be waitcont to be true again, is the same for all detector bucles

void help();
void processVideo(char* videoFilename);
void help() { cout  << "--------------------------------------------------------------------------" << endl
                    << "This program shows how to use background subtraction methods provided by "  << endl
                    << " OpenCV. You can process both videos (-vid)."                               << endl
                                                                                                    << endl
                    << "Usage:"                                                                     << endl
                    << "./bs {-vid <video filename>|-img <image filename>}"                         << endl
                    << "for example: ./bs -vid video.avi"                                           << endl
                    << "press \"p\" to pause video (5 seconds)"                                     << endl
                    << "press \"q\" to quit video"                                                  << endl
                    << "--------------------------------------------------------------------------" << endl
                    << endl;
}
int main(int argc, char* argv[]) {
    //print help information
    help();
    //check for the input parameter correctness
    if(argc != 3) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }
    //create GUI windows
    namedWindow("Frame");
    namedWindow("FG Mask MOG 2");
    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    if(strcmp(argv[1], "-vid") == 0) {
        //input data coming from a video
        processVideo(argv[2]);

    } else {
        //error in reading input parameters
        cerr <<"Please, check the input parameters." << endl;
        cerr <<"Exiting..." << endl;
        return EXIT_FAILURE;
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}
void processVideo(char* videoFilename) {
    //create the capture object
    VideoCapture capture(videoFilename);//VideoCapture(0) for integrated camera 
                //videoFilename for a file / VideoCapture(1) for plugged camera
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        if ((char)keyboard == 'p') {//pausa de 5 seg
            Sleep(5000);
        }
        
        //update the background model
        pMOG2->apply(frame, fgMaskMOG2);//this is the right momment when subtraction
                                        //is aplicated
        Mat blurry;
        medianBlur(fgMaskMOG2, blurry, 9);//blur the image to avoid noise
        //cvtColor(fgMaskMOG2, fgMaskMOG2, COLOR_GRAY2BGR);
        //we dont need to change to gray beacuse pMOG returns a grey image
        Mat binario;
        threshold(blurry, binario, 222, 255, THRESH_BINARY);
        //converts image to 2 colors image because its easier to detect persons
        
               
        //circulo (frame->nombre Point(x,y)tamaño scalar->color(b,g,r)grosor)//circle print syntaxis
        //circle(frame, Point(200, 100), 50, Scalar(0,255,0), 1);//circle example
        zoneAXL = ((frame.cols / 4) - ((((frame.cols * percentX) / 100) / 2) / 2));
        zoneAYU = (((frame.rows / 2) - ((frame.rows * percentY) / 100) / 2));
        zoneAXR = ((frame.cols / 4) + ((((frame.cols * percentX) / 100) / 2) / 2));
        zoneAYD = (((frame.rows / 2) + ((frame.rows * percentY) / 100) / 2));
        zoneBXL = ((frame.cols * 3 / 4) - ((((frame.cols * percentX) / 100) / 2) / 2));
        zoneBYU = (((frame.rows / 2) - ((frame.rows * percentY) / 100) / 2));
        zoneBXR = ((frame.cols * 3 / 4) + ((((frame.cols * percentX) / 100) / 2) / 2));
        zoneBYD = (((frame.rows / 2) + ((frame.rows * percentY) / 100) / 2));
        //notice zoneAYD and zoneBYD is the same but I use both for a better comprehension
	
        //rectangle(frame, Point(x, y), Point(x, y), scalar->color(b,g,r)Thickness); //rectangle syntaxis
        rectangle(frame, Point(zoneAXL, zoneAYU), Point(zoneAXR, zoneAYD),
                Scalar(0, 255, 0),2);//rectangle right original video
        rectangle(frame, Point(zoneBXL, zoneBYU), Point(zoneBXR, zoneBYD),
                Scalar(255, 100, 0),2);//rectangle left original  video
        rectangle(binario, Point(zoneAXL, zoneAYU), Point(zoneAXR, zoneAYD),
                Scalar(255, 100, 0),2);//rectangle left subtracted video
        rectangle(binario, Point(zoneBXL, zoneBYU), Point(zoneBXR, zoneBYD),
                Scalar(255, 100, 0),2);//rectangle left subtracted video
        
        ////////////////////////////////relative to edges window
        Mat canny_output;//declare a variable to store the result of detecting edges by using canny
        vector<vector<Point> > contours;//this is to store contours as verctor points
        vector<Vec4i> hierarchy;//this is to store hiterachy information
        
        /// Detect edges using canny. 
        Canny(//The function finds edges in the input image image and marks them
              //in the output map edges using the Canny algorithm. The smallest 
              //value between threshold1 and threshold2 is used for edge linking. The largest
              //value is used to find initial segments of strong edges
                binario,     //source 8-bit input image
                canny_output,//output edge map; single channels 8-bit image, which has the same size as image
                (0,0),       //	first threshold for the hysteresis procedure
                (0,0)*2,     //second threshold for the hysteresis procedure
                3);          //aperture size for the Sobel operator
        
        /// Find contours
        findContours(//uses the results of canny above
                canny_output,       //Image: source image
                contours,           //Contours: Detected contours(on Canny function above) Each contour is stored as a vector of points
                hierarchy,          //Hiterachy: Optional output vector, containing information about the image topology. It has as many
                                    //elements as the number of contours. For each i-th contour contours[i] , the elements hierarchy[i][0] ,
                                    //hiearchy[i][1] , hiearchy[i][2] , and hiearchy[i][3] are set to 0-based indices in contours of the
                                    //next and previous contours at the same hierarchical level, the first child contour and the parent
                                    //contour, respectively. If for the contour i there are no next, previous, parent, or nested contours,
                                    //the corresponding elements of hierarchy[i] will be negative
                RETR_TREE,          //Mode: Contour retrieval mode (this option retrieves only the extreme outer contours. It sets
                                    //hierarchy[i][2]=hierarchy[i][3]=-1 for all the contours) for more info see cv::RetrievalModes on
                                    //http://docs.opencv.org/3.0-rc1/d3/dc0/group_...
                CHAIN_APPROX_SIMPLE,//Method: Contour retrieval mode, see cv::RetrievalModes we use this mode because it give only iportant
                                    //points and we save memory
                Point(0, 0) );      //Offset: Optional offset by which every contour point is shifted. This is useful if the contours are
                                    //extracted from the image ROI and then they should be analyzed in the whole image context.

        /// Get the moments
        vector<Moments> mu(contours.size() );//mu[i] is vector moment 
                                    //(the mass that we use later to find the center of it)
        for( int i = 0; i < contours.size(); i++ ) {
            mu[i] = moments( contours[i], false );
        }
        
        ///  Get the mass centers:
        vector<Point2f> mc( contours.size() );
        for( int i = 0; i < contours.size(); i++ ){
            mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );//mc[i] is the
                                                //mass center of every moving object IMPORTANT!!
            
            //this detect zone A up bounds
            if ((sigueau)//sigue sets true because a point of mass was near at the before frame
                    //(if we are here we are not at the first frame since detection)
                    &&(refXau + ((frame.cols * 3) / 100) > mu[i].m10/mu[i].m00)
                    //3 is a percent value to evaluate if the center of mass we manage
                    //is near to the one witch activated "sigue"
                    &&(refXau - ((frame.cols * 3) / 100) < mu[i].m10/mu[i].m00)
                    &&(refYau - ((frame.rows * 2) / 100) < mu[i].m01/mu[i].m00)
                    &&(refYau + ((frame.rows * 22) / 100) > mu[i].m01/mu[i].m00)){
                //after these four ifs we can know if the point is inside of an imaginary
                //rectangle around the center of mass
                cercaau = true;
                nuevoau = mu[i].m01/mu[i].m00;
            } else {
                cuentaau++;
                if (cuentaau > 10){
                    sigueau = false;
                    cercaau = false;
                    cuentaau = 0;
                }
            }
            if((sigueau)&&(cercaau)&&(waitau)){//if following (one of the next 10 frames) is near then...
                if ((refYau < nuevoau)) {
                    au++;
                    waitau = false;
                    sigueau = false;
                    cercaau = false;
                    cuentaau = 0;
                }
            }
            if((!sigueau)//detects if an object is between the bound or little inside
                    &&(mu[i].m01/mu[i].m00 > zoneAYU)
                    &&(mu[i].m01/mu[i].m00 < (zoneAYU + ((frame.rows * 5) / 100)))
                    &&(zoneAXL < mu[i].m10/mu[i].m00)&&(zoneAXR > mu[i].m10/mu[i].m00)){
                refXau = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYau = mu[i].m01/mu[i].m00;//save the first point inside y axis
                sigueau = true;
            }
        /////////////////////////////////
            
            //this detect zone A left bounds
            if ((sigueal)                                                           //sigue is true because a point of mass was near at the before frame(now we not at the first frame since detection)
                    &&(refXal + ((frame.cols * 22) / 100) > mu[i].m10/mu[i].m00)   //9.3 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXal - ((frame.cols * 2) / 100) < mu[i].m10/mu[i].m00)
                    &&(refYal + ((frame.rows * 3) / 100) > mu[i].m01/mu[i].m00)
                    &&(refYal - ((frame.rows * 3) / 100) < mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercaal = true;
                nuevoal = mu[i].m10/mu[i].m00;
            } else {
                cuentaal++;
                if (cuentaal > 10){
                    
                    sigueal = false;
                    cercaal = false;
                    cuentaal = 0;
                }
            }
            if((sigueal)&&(cercaal)&&(waital)){//if following (one of the next 10 frames) is near then...
                if ((refXal < nuevoal)) {
                    al++;
                    waital = false;
                    sigueal = false;
                    cercaal = false;
                    cuentaal = 0;
                }
            }
            if((!sigueal)&&(mu[i].m10/mu[i].m00 > zoneBXL)&&(mu[i].m10/mu[i].m00 < (zoneBXL + ((frame.cols * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneBYU < mu[i].m01/mu[i].m00)&&(zoneBYD > mu[i].m01/mu[i].m00)){
                refXal = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYal = mu[i].m01/mu[i].m00;//save the first point inside y axis
                sigueal = true;

            }
        /////////////////////////////////
            
            //this detect zone A right bounds
            //the following if works ok for 640 x 360 resolution but we change pixels for % so that it can work with all resolutions
            //if ((sigue)&&(refXar - 60 < mu[i].m10/mu[i].m00)&&(refXar + 10 > mu[i].m10/mu[i].m00)&&(refY - 60 < mu[i].m10/mu[i].m00)&&(refY + 60 > mu[i].m10/mu[i].m00)){
            if ((siguear)                                                           //sigue is true because a point of mass was near at the before frame
                    &&(refXar - ((frame.cols * 22) / 100) < mu[i].m10/mu[i].m00)   //9.3 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXar + ((frame.cols * 2) / 100) > mu[i].m10/mu[i].m00)
                    &&(refYar - ((frame.rows * 3) / 100) < mu[i].m01/mu[i].m00)
                    &&(refYar + ((frame.rows * 3) / 100) > mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercaar = true;
                nuevoar = mu[i].m10/mu[i].m00;
            } else {
                cuentaar++;
                if (cuentaar > 10){
                    siguear = false;
                    cercaar = false;
                    cuentaar = 0;
                }
            }
            if((siguear)&&(cercaar)&&(waitar)){
                if ((refXar > nuevoar)) {
                    ar++;
                    waitar = false;
                    siguear = false;
                    cercaar = false;
                    cuentaar = 0;
                }
            }
            if((!siguear)&&(mu[i].m10/mu[i].m00 < zoneAXR)&&(mu[i].m10/mu[i].m00 > (zoneAXR - ((frame.cols * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneAYU < mu[i].m01/mu[i].m00)&&(zoneAYD > mu[i].m01/mu[i].m00)){
                refXar = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYar = mu[i].m01/mu[i].m00;//save the first point inside y axis
                siguear = true;
            }
        ////////////////////
            
        //this detect zone A down bounds
            if ((siguead)                                                         //sigue is true because a point of mass was near at the before frame(now we not at the first frame since detection)
                    &&(refXad + ((frame.cols * 3) / 100) > mu[i].m10/mu[i].m00)  //22 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXad - ((frame.cols * 3) / 100) < mu[i].m10/mu[i].m00)
                    &&(refYad + ((frame.rows * 2) / 100) > mu[i].m01/mu[i].m00)
                    &&(refYad - ((frame.rows * 22) / 100) < mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercaad = true;
                nuevoad = mu[i].m01/mu[i].m00;
            } else {
                cuentaad++;
                if (cuentaad > 10){
                    siguead = false;
                    cercaad = false;
                    cuentaad = 0;
                }
            }
            if((siguead)&&(cercaad)&&(waitad)){//if following (one of the next 10 frames) is near then...
                if ((refYad > nuevoad)) {
                    ad++;
                    waitad = false;
                    siguead = false;
                    cercaad = false;
                    cuentaad = 0;
                }
            }
            if((!siguead)&&(mu[i].m01/mu[i].m00 < zoneAYD)&&(mu[i].m01/mu[i].m00 > (zoneAYD - ((frame.rows * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneAXL < mu[i].m10/mu[i].m00)&&(zoneAXR > mu[i].m10/mu[i].m00)){
                refXad = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYad = mu[i].m01/mu[i].m00;//save the first point inside y axis
                siguead = true;
            }
        /////////////////////////////////
            
        //this detect zone B up bounds
            if ((siguebu)                                                         //sigue is true because a point of mass was near at the before frame(now we not at the first frame since detection)
                    &&(refXbu + ((frame.cols * 3) / 100) > mu[i].m10/mu[i].m00)  //22 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXbu - ((frame.cols * 3) / 100) < mu[i].m10/mu[i].m00)
                    &&(refYbu - ((frame.rows * 2) / 100) < mu[i].m01/mu[i].m00)
                    &&(refYbu + ((frame.rows * 22) / 100) > mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercabu = true;
                nuevobu = mu[i].m01/mu[i].m00;
            } else {
                cuentabu++;
                if (cuentabu > 10){
                    siguebu = false;
                    cercabu = false;
                    cuentabu = 0;
                }
            }
            if((siguebu)&&(cercabu)&&(waitbu)){//if following (one of the next 10 frames) is near then...
                if ((refYbu < nuevobu)) {
                    bu++;
                    waitbu = false;
                    siguebu = false;
                    cercabu = false;
                    cuentabu = 0;
                }
            }
            if((!siguebu)&&(mu[i].m01/mu[i].m00 > zoneBYU)&&(mu[i].m01/mu[i].m00 < (zoneBYU + ((frame.rows * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneBXL < mu[i].m10/mu[i].m00)&&(zoneBXR > mu[i].m10/mu[i].m00)){
                refXbu = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYbu = mu[i].m01/mu[i].m00;//save the first point inside y axis
                siguebu = true;
            }
        /////////////////////////////////
            
        //this detect zone B left bounds
            if ((siguebl)                                                           //sigue is true because a point of mass was near at the before frame(now we not at the first frame since detection)
                    &&(refXbl + ((frame.cols * 22) / 100) > mu[i].m10/mu[i].m00)   //9.3 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXbl - ((frame.cols * 2) / 100) < mu[i].m10/mu[i].m00)
                    &&(refYbl + ((frame.rows * 3) / 100) > mu[i].m01/mu[i].m00)
                    &&(refYbl - ((frame.rows * 3) / 100) < mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercabl = true;
                nuevobl = mu[i].m10/mu[i].m00;
            } else {
                cuentabl++;
                if (cuentabl > 10){
                    siguebl = false;
                    cercabl = false;
                    cuentabl = 0;
                }
            }
            if((siguebl)&&(cercabl)&&(waitbl)){
                if ((refXbl < nuevobl)) {
                    bl++;
                    waitbl = false;
                    siguebl = false;
                    cercabl = false;
                    cuentabl = 0;
                }
            }
            if((!siguebl)&&(mu[i].m10/mu[i].m00 > zoneBXL)&&(mu[i].m10/mu[i].m00 < (zoneBXL + ((frame.cols * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneBYU < mu[i].m01/mu[i].m00)&&(zoneBYD > mu[i].m01/mu[i].m00)){
                refXbl = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYbl = mu[i].m01/mu[i].m00;//save the first point inside y axis
                siguebl = true;
            }
        /////////////////////////////////
            
        //this detect zone B right bounds
            if ((siguebr)                                                           //sigue is true because a point of mass was near at the before frame
                    &&(refXbr - ((frame.cols * 22) / 100) < mu[i].m10/mu[i].m00)   //9.3 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXbr + ((frame.cols * 2) / 100) > mu[i].m10/mu[i].m00)
                    &&(refYbr - ((frame.rows * 3) / 100) < mu[i].m01/mu[i].m00)
                    &&(refYbr + ((frame.rows * 3) / 100) > mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercabr = true;
                nuevobr = mu[i].m10/mu[i].m00;
            } else {
                cuentabr++;
                if (cuentabr > 10){
                    siguebr = false;
                    cercabr = false;
                    cuentabr = 0;
                }
            }
            if((siguebr)&&(cercabr)&&(waitbr)){
                if ((refXbr > nuevobr)) {
                    br++;
                    waitbr = false;
                    siguebr = false;
                    cercabr = false;
                    cuentabr = 0;
                }
            }
            if((!siguebr)&&(mu[i].m10/mu[i].m00 < zoneBXR)&&(mu[i].m10/mu[i].m00 > (zoneBXR - ((frame.cols * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneBYU < mu[i].m01/mu[i].m00)&&(zoneBYD > mu[i].m01/mu[i].m00)){
                refXbr = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYbr = mu[i].m01/mu[i].m00;//save the first point inside y axis
                siguebr = true;
            }
        /////////////////////////////////
            
        //this detect zone B down bounds
            if ((siguebd)                                                         //sigue is true because a point of mass was near at the before frame(now we not at the first frame since detection)
                    &&(refXbd + ((frame.cols * 3) / 100) > mu[i].m10/mu[i].m00)  //22 is a percent value to evaluate if the center of mass we manage is near to the one witch activated "sigue"
                    &&(refXbd - ((frame.cols * 3) / 100) < mu[i].m10/mu[i].m00)
                    &&(refYbd + ((frame.rows * 2) / 100) > mu[i].m01/mu[i].m00)
                    &&(refYbd - ((frame.rows * 22) / 100) < mu[i].m01/mu[i].m00)){//after these four ifs we can know if the point is inside of an iimaginary rectangle around the center of mass
                cercabd = true;
                nuevobd = mu[i].m01/mu[i].m00;
            } else {
                cuentabd++;
                if (cuentabd > 10){
                    siguebd = false;
                    cercabd = false;
                    cuentabd = 0;
                }
            }
            if((siguebd)&&(cercabd)&&(waitbd)){//if following (one of the next 10 frames) is near then...
                if ((refYbd > nuevobd)) {
                    bd++;
                    waitbd = false;
                    siguebd = false;
                    cercabd = false;
                    cuentabd = 0;
                }
            }
            if((!siguebd)&&(mu[i].m01/mu[i].m00 < zoneBYD)&&(mu[i].m01/mu[i].m00 > (zoneBYD - ((frame.rows * 5) / 100)))//detect if an object is on the bound or little inside
            &&(zoneBXL < mu[i].m10/mu[i].m00)&&(zoneBXR > mu[i].m10/mu[i].m00)){
                refXbd = mu[i].m10/mu[i].m00;//save the first point inside x axis
                refYbd = mu[i].m01/mu[i].m00;//save the first point inside y axis
                siguebd = true;
            }
        /////////////////////////////////
            
        }//here ends the for which contains 
        
        if (!waitau) {//wait is used to uneneable the counter of people for a portion of second
                                                 //to avoid count the same person several times
            waitcontau++;
        }
        if (waitcontau >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitau = true;
            waitcontau = 0;
        }
        
        if (!waital) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontar++;
        }
        if (waitcontal >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waital = true;
            waitcontal = 0;
        }
        
        if (!waitar) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontar++;
        }
        if (waitcontar >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitar = true;
            waitcontar = 0;
        }
        
        if (!waitad) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontad++;
        }
        if (waitcontad >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitad = true;
            waitcontad = 0;
        }
        
        if (!waitbu) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontbu++;
        }
        if (waitcontbu >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitbu = true;
            waitcontbu = 0;
        }
        
        if (!waitbl) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontbl++;
        }
        if (waitcontbl >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitbl = true;
            waitcontbl = 0;
        }
        
        if (!waitbr) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontbr++;
        }
        if (waitcontbr >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitbr = true;
            waitcontbr = 0;
        }
        
        if (!waitbd) {//wait is used to uneneable the counter of people for a portion of second to avoid count the same person several times
            waitcontbd++;
        }
        if (waitcontbd >= waitvalue){//the time we uneneable the counter is a number of frames contained on wait
            waitbd = true;
            waitcontbd = 0;
        }
       
        
        
        stringstream at; //this 3 lines are to convert int to string
        at << ar + al + ad + au;
        string str = at.str();
        putText(frame, str, Point(zoneAXR - 20, ((frame.rows * 70) / 100)),
                        CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        
        stringstream bt; //this 3 lines are to convert int to string
        bt << bu + br + bl + bd;
        string strr = bt.str();
        putText(frame, strr, Point(zoneBXR - 20, ((frame.rows * 70) / 100)),
                        CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2);
        //this print total count people ( zona A + B) on the screen
        stringstream total; //this 3 lines are to convert int to string
        total << ar + al + ad + bu + br + bl + bd;
        string strt = total.str();
        putText(frame, "both: " + strt, Point(((frame.cols * 40) / 100), ((frame.rows * 8) / 100)),
                        CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
        
        /// Draw contours
        RNG rng(12345);//generates a random number to create colors
        Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );//create drawing variable
            //with properties of canny_output size:(cols, rows) type: Created matrix type.
        for( int i = 0; i < contours.size(); i++ ) {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours(
                    drawing,//image which draw
                    contours,//All the input contours. Each contour is stored as a point vector
                    i,//Parameter indicating a contour to draw.If it is negative, all the contours are drawn
                    color,//color of the contours
                    2,//thickness: Thickness of lines the contours are drawn with. If it is negative
                              //(for example, thickness=CV_FILLED ), the contour interiors are drawn
                    8,//line conectivity
                    hierarchy,//heritage
                    2,//maxLevel: Maximal level for drawn contours. If it is 0, only the specified
                      //contour is drawn. If it is 1, the function draws the contour(s) and all the
                      //nested contours and so on
                    Point() );// offset
            circle(//draw a circle just in the middle of the contours
                    drawing,//image to print
                    mc[i],//point to print(x,y) mc[i] is the center of the contours calculated before
                    1,//radius
                    color,//color
                    -1,//thickness
                    8,//lineType
                    0 );//Number of fractional bits in the coordinates of the center and in the radius value.
        }
        
        /// Show in a window
        namedWindow( "Contours", WINDOW_AUTOSIZE );
        imshow( "Contours", drawing );
         //////////////////////////////////////////////////////////////////
        
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask MOG 2", binario);
        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    
    //delete capture object
    capture.release();
}




