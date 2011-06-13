#ifndef __DESCRIPTORS_COMMON_H__
#define __DESCRIPTORS_COMMON_H__
/*
 * =====================================================================================
 *
 *       Filename:  descriptor.h
 *
 *    Description: 
 *        Version:  1.0
 *        Created:  06/07/2011 04:06:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Stephen Miller
 *        Company:  UC Berkeley
 *
 * =====================================================================================
 */

#include <patch_vision/extraction/descriptor.h>
#include <patch_vision/thirdparty/LBP.h>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using cv::Ptr;
using cv::DescriptorExtractor;
//using cv::HOGDescriptor;
typedef cv::HOGDescriptor HOGDescriptor_cv;

class LBPDescriptor : public Descriptor{
    public:

        LBPDescriptor( int patch_size );
        ~LBPDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;
        
    private:
        int _patch_size;
};

class HOGDescriptor : public Descriptor{
    public:

        HOGDescriptor( int patch_size );
        ~HOGDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;
        
    private:
        int _patch_size;
        HOGDescriptor_cv* _descriptor;
};


class StackedDescriptor : public Descriptor{
    public:
        
        StackedDescriptor( vector<Descriptor*> descriptors, vector<float> weights );
        ~StackedDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        void process_image( const Mat &image, vector<vector<float> > &features, 
                            vector< PatchDefinition* > &patch_definitions, const PatchMaker &pm, 
                            bool verbose=false );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;

    private:
        vector<Descriptor*> _descriptors;
        vector<float> _weights;
};

class ColoredDescriptor : public Descriptor{
    public:
        
        ColoredDescriptor( Descriptor* bw_descriptor, ColorMode color_mode );
        ~ColoredDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;

    private:
        Descriptor* _bw_descriptor;
        ColorMode _color_mode;

};

class RotatedDescriptor : public Descriptor{
    public:
        
        RotatedDescriptor( Descriptor* bw_descriptor );
        ~RotatedDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;

    private:
        Descriptor* _bw_descriptor;

};

class HueHistogramDescriptor : public Descriptor{
    public:

        HueHistogramDescriptor( int patch_size, int num_bins );
        ~HueHistogramDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;
        
    private:
        int _patch_size;
        int _num_bins;

};

void soft_histogram( const vector<float> &values, vector<float> &output, int num_bins, float min_val, float max_val, bool cycle );

float float_mod(float a, float b);
int int_mod(int a, int b);

class SIFTDescriptor : public Descriptor{
    public:
        SIFTDescriptor( int patch_size );
        ~SIFTDescriptor( );

        void process_patch( const Mat &patch, vector<float> &feature );
        void process_image( const Mat &image, vector<vector<float> > &features, 
                            vector< PatchDefinition* > &patch_definitions, const PatchMaker &pm, 
                            bool verbose=false );
        string name( ) const;
        int descriptor_size( ) const;
        int patch_size( ) const;

        ColorMode required_color_mode( ) const;

    private:
        int _patch_size;
        Ptr<DescriptorExtractor> _descriptor_extractor;
        
};

#endif
