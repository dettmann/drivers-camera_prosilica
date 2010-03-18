/* 
 * File:   CameraProsilica.h
 * Author: Alexander Duda
 *
 * Created on February 9, 2010, 8:39 AM
 */

#ifndef _CAMGIGEPROSILICA_H
#define	_CAMGIGEPROSILICA_H

#include "camera_interface/CamInterface.h"
#include "arch.h"
#include "PvApi.h"
#include <list>

namespace camera
{
    struct ProFrame
    {
    private:
        std::vector<uint8_t> data;
    public:
        tPvFrame frame;
        ProFrame( int frame_size_in_byte)
        {
            resize(frame_size_in_byte);
        }
        
        inline void swap(Frame &other)
        {
            if(data.size() != other.image.size())
                 throw std::runtime_error("Frame size mismatch. "
                                          " Can not swap data.");
            other.image.swap(data);
            frame.ImageBuffer = &data[0];
        }

        inline void resize( int frame_size_in_byte)
        {
            data.resize(frame_size_in_byte);
            frame.ImageBuffer = &data[0];
            frame.ImageBufferSize = frame_size_in_byte;
            frame.AncillaryBuffer     = NULL;

           //using AncillaryBufferSize to indicate if frame is done
           //0 --> frame is done
           //1 --> frame is not done
           //faster than PvCaptureWaitForFrameDone
           frame.AncillaryBufferSize = 0;
        } 
    };

    class CamGigEProsilica : public CamInterface
    {
    private:
        tPvHandle camera_handle_;
        CamInfo cam_info_;
        static int instance_count_;
        std::list<ProFrame*> frame_queue_;
        int frame_size_in_byte_;
        AccessMode access_mode_;
	void (*pcallback_function_)(const void* p);
	void *pass_through_pointer_;

    public:
        CamGigEProsilica();
        virtual ~CamGigEProsilica();

    public:
        int listCameras(std::vector<CamInfo>&cam_infos)const;
        bool open(const CamInfo &cam,const AccessMode mode);
        bool isOpen()const;
        bool close();

        bool grab(const GrabMode mode, const int buffer_len);
        bool retrieveFrame(Frame &frame,const int timeout);
        bool setFrameSettings(const frame_size_t size,
                                      const frame_mode_t mode,
                                      const  uint8_t color_depth,
                                      const bool resize_frames);
        bool isFrameAvailable();
        int skipFrames();

        bool setAttrib(const int_attrib::CamAttrib attrib,const int value);
        bool setAttrib(const double_attrib::CamAttrib attrib,const double value);
        bool setAttrib(const enum_attrib::CamAttrib attrib);
        bool setAttrib(const str_attrib::CamAttrib attrib,const std::string &string);

        bool isAttribAvail(const int_attrib::CamAttrib attrib);
        bool isAttribAvail(const str_attrib::CamAttrib attrib);
        bool isAttribAvail(const double_attrib::CamAttrib attrib);
        bool isAttribAvail(const enum_attrib::CamAttrib attrib);

        int getAttrib(const int_attrib::CamAttrib attrib);
        double getAttrib(const double_attrib::CamAttrib attrib);
        std::string getAttrib(const str_attrib::CamAttrib attrib);
        bool isAttribSet(const enum_attrib::CamAttrib attrib);

        bool getFrameSettings(frame_size_t &size,frame_mode_t &mode,uint8_t &color_depth);

        bool triggerFrame();
	
	bool setCallbackFcn(void (*pcallback_function)(const void* p),void *p);
	void callUserCallbackFcn()const;
	
    private:
        //helpers
        void copy_tPvCameraInfo_To_tCamInfo
                        (const tPvCameraInfo& source, CamInfo& dest) const;
        bool fillCameraIpSettings(CamInfo& cam)const;
        bool prepareQueueForGrabbing(const int queue_len);
        inline bool isFrameQueued(const ProFrame* frame);
        inline bool queueFrame(ProFrame* frame);
        static void callBack(tPvFrame * frame);
	static void callBack2(tPvFrame * frame);
    };
}


#endif	/* _CAMGIGEPROSILICA_H */

