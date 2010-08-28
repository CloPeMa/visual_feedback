#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "sensor_msgs/CameraInfo.h"
#include "image_transport/image_transport.h"
#include "cv_bridge/CvBridge.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "image_processor/ProcessBridge.h"
#include "SocksDemo/GripPointDetector.h"
#include "SocksDemo/SockStateDetector.h"

sensor_msgs::CvBridge bridge_;
SockStateDetector* inside_out_detector;
GripPointDetector* grip_point_detector;


IplImage *find_grip_point_process(IplImage *input_cv_image, CvPoint2D64f* pts2d, double *params, int& num_pts, int& num_params){
    num_pts = 2;
    num_params = 2;
    //Call grip_point_detector
    ROS_INFO("Calling grip_point_detector...");
    cvSaveImage("/tmp/grip_point_detector_img.png",input_cv_image);
    ROS_INFO("Successfully wrote image...");
    input_cv_image = cvLoadImage("/tmp/grip_point_detector_img.png");
    ROS_INFO("Successfully loaded image...");
    GripPointResult result = grip_point_detector->process(input_cv_image);
    ROS_INFO("Successfully called grip_point_detector...");
    pts2d[0].x = result.leftGripPoint.x;
    pts2d[0].y = result.leftGripPoint.y;
    pts2d[1].x = result.rightGripPoint.x;
    pts2d[1].y = result.rightGripPoint.y;
    params[0] = result.leftApproachAngle;
    params[1] = result.rightApproachAngle;
    ROS_INFO("Ready to return the annotatedImage...");
    return result.annotatedImage;
}

IplImage *is_inside_out_process(IplImage *input_cv_image, CvPoint2D64f* pts2d, double *params, int& num_pts, int& num_params,IplImage *input_cv_image2){
    num_pts = 0;
    num_params = 2;

    //Call inside_out_detector
    ROS_INFO("Calling inside_out_detector...");
    SockStateResult result = inside_out_detector->process(input_cv_image,input_cv_image2);
    ROS_INFO("Called inside_out_detector...");
    if (result.isLeftInsideOut){
        params[0] = 1.0;
    }
    else{
        params[0] = -1.0;
    }
    if (result.isRightInsideOut){
        params[1] = 1.0;
    }
    else{
        params[1] = -1.0;
    }

    return input_cv_image;
    
}

bool find_grip_point(image_processor::ProcessBridge::Request    &req,
                 image_processor::ProcessBridge::Response &res )
{
    sensor_msgs::Image image = req.image;
    sensor_msgs::ImagePtr img_ptr(new sensor_msgs::Image(image));
    sensor_msgs::CameraInfo cam_info = req.info;
    
    IplImage *cv_image = NULL;    
    try
        {
                cv_image = bridge_.imgMsgToCv(img_ptr, "bgr8");
        }
        catch (sensor_msgs::CvBridgeException error)
        {
                ROS_ERROR("error");
                return false;
        }
        
        //Crop image
        int height = 2050;
        int width = 2448;
        cvSetImageROI(cv_image,cvRect(0,1100,width,height-1100));
        IplImage* cropped_image = cvCloneImage(cv_image);
        CvPoint2D64f temp_pts[128];
        double temp_params[128];
        
        IplImage *output_cv_image;
        int num_pts = 0;
        int num_params = 0;
        
        ROS_INFO("Ready to call find_grip_point...");
        output_cv_image = find_grip_point_process(cropped_image,temp_pts,temp_params,num_pts,num_params);
        

        for( int i = 0; i < num_pts; i++){
            res.pts_x.push_back(temp_pts[i].x);
            res.pts_y.push_back(temp_pts[i].y+1100);
        }
        for ( int i = 0; i < num_params; i++){
           res.params.push_back(temp_params[i]);
        }
        sensor_msgs::ImagePtr output_img_ptr;
        sensor_msgs::Image output_img;
        try
        {
                output_img_ptr = bridge_.cvToImgMsg(output_cv_image, "bgr8");
                output_img = *output_img_ptr;
        }
        catch (sensor_msgs::CvBridgeException error)
        {
                ROS_ERROR("error");
                return false;
        }
    res.image_annotated = output_img;

    return true;
}

bool is_inside_out(image_processor::ProcessBridge::Request    &req,
                 image_processor::ProcessBridge::Response &res )
{
    sensor_msgs::Image image = req.image;
    sensor_msgs::Image image2 = req.image2;
    sensor_msgs::ImagePtr img_ptr(new sensor_msgs::Image(image));
    
    sensor_msgs::CameraInfo cam_info = req.info;
    
    IplImage *cv_image;    
    IplImage *cv_image2;   
    try
        {
                cv_image = bridge_.imgMsgToCv(img_ptr, "bgr8");
        }
        catch (sensor_msgs::CvBridgeException error)
        {
                ROS_ERROR("error");
                return false;
        }
   
        CvPoint2D64f temp_pts[128];
        double temp_params[128];
        //Crop image
        int height = 2050;
        int width = 2448;
        cvSetImageROI(cv_image,cvRect(0,1100,width,height-1100));
        IplImage* cropped_image = cvCloneImage(cv_image);
        cvSaveImage("/home/stephen/cropped_image1.png",cropped_image);
        sensor_msgs::ImagePtr img2_ptr(new sensor_msgs::Image(image2));
        try{
                cv_image2 = bridge_.imgMsgToCv(img2_ptr, "bgr8");
        }

        catch (sensor_msgs::CvBridgeException error)
        {
                ROS_ERROR("error");
                return false;
        }
        cvSetImageROI(cv_image2,cvRect(0,1100,width,height-1100));
        IplImage* cropped_image2 = cvCloneImage(cv_image2);
        
        cvSaveImage("/home/stephen/cropped_image2.png",cropped_image2);
        cvReleaseImage(&cropped_image);
        cvReleaseImage(&cropped_image2);
        IplImage* new_cropped_image = cvLoadImage("/home/stephen/cropped_image1.png");
        IplImage* new_cropped_image2 = cvLoadImage("/home/stephen/cropped_image2.png");
        IplImage *output_cv_image;
        int num_pts = 0;
        int num_params = 0;
        output_cv_image = is_inside_out_process(new_cropped_image,temp_pts,temp_params,num_pts,num_params,new_cropped_image2);
        

        for( int i = 0; i < num_pts; i++){
            res.pts_x.push_back(temp_pts[i].x);
            res.pts_y.push_back(temp_pts[i].y);
        }
        for ( int i = 0; i < num_params; i++){
           res.params.push_back(temp_params[i]);
        }
        sensor_msgs::ImagePtr output_img_ptr;
        sensor_msgs::Image output_img;
        try
        {
                output_img_ptr = bridge_.cvToImgMsg(output_cv_image, "bgr8");
                output_img = *output_img_ptr;
        }
        catch (sensor_msgs::CvBridgeException error)
        {
                ROS_ERROR("error");
                return false;
        }
    res.image_annotated = output_img;

    return true;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "cpp_image_processing_node");
    ros::NodeHandle n;
    inside_out_detector = new SockStateDetector();
    grip_point_detector = new GripPointDetector();
    ros::ServiceServer grip_point_service = n.advertiseService("find_grip_point", find_grip_point);
    ros::ServiceServer inside_out_service = n.advertiseService("is_inside_out", is_inside_out);
    ROS_INFO("Ready to bridge cpp and python...");
    ros::spin();

    return 0;
}

