/*
This module was prepared as a part of the I2V Software.

Following functions were contributed by me

1.void rotate_on_click_1X4(filter_t *p_filter, IplImage* input, IplImage* output, int x_old, int y_old, int x_diff, int y_diff);
2.void zoom_perspective(filter_t *p_filter, IplImage *output, IplImage *input, double radius, double phi, int position, double zoom);
3.void create_Perspective(filter_t *p_filter, IplImage* output, IplImage *input, double theta1, double theta2, int position);
4.void zoom_on_click(filter_t *, IplImage* input, IplImage* output, int point_x, int point_y);
5.static int Mouse(filter_t *, vlc_mouse_t *, const vlc_mouse_t *, const vlc_mouse_t *);
6.static int MouseWheelCallback(vlc_object_t *, char const *,vlc_value_t, vlc_value_t, void *);
7.double HOME_MADE_ATOI(string temp);
8.void savedata(filter_t *p_filter);
9.void savepreset(filter_sys_t *p_sys, char* preset_id);
10.void readpreset(filter_sys_t *p_sys, char* preset_id);

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_filter.h>
#include <vlc_vout.h>
#include "filter_common.h"
#include <vlc_image.h>
#include "filter_event_info.h"
#include <stdio.h>

#include <cxcore.h>
#include <cv.h>
#include<dir.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <vlc_fs.h>
#include "vaUtils.h"
#include "vlc/libvlc_structures.h"

/*****************************************************************************
 * filter_sys_t : filter descriptor
 *****************************************************************************/
struct filter_sys_t {
    CvMemStorage* p_storage;
    CvHaarClassifierCascade* p_cascade;
    video_filter_event_info_t event_info;
    int i_id;
    vaUtils *obj;
    analytic_input_params * input_parameters;
    int counter;

    // Defining Required Variables
    int point_x;
    int point_y;
    int diff_x;
    int diff_y;
    int x_old;
    int y_old;
    int x_new;
    int y_new;
    int mov;
    int click;
    double theta, angle_rotate;
    double r;
    double radius1;
    double phi1;
    double radius2;
    double phi2;
    double radius3;
    double phi3;
    double zoom_1X4_1;
    double zoom_1X4_2;
    double zoom_1X4_3;
    double zoom_on_click_zoom;
    int para;
    int start;
    double minimum;
    float scroll_value;
    double centre_x, centre_y, radius_video, radius_video_check, radius_video_temp;
    int output_height, output_width;
    bool Var_on_click_zoom;
    int point_x_on_click_zoom;
    int point_y_on_click_zoom;
    double on_click_radius;
    double on_click_theta;
    char camera_id[50];
    bool save_preset;
    double f_scale;
    int output_max_width;
    int output_max_height;
};

struct EnumData {
    DWORD dwProcessId;
    HWND hWnd;
};

/****************************************************************************
 * Local prototypes
 ****************************************************************************/
static int OpenFilter(vlc_object_t *);
static void CloseFilter(vlc_object_t *);

static picture_t *Filter(filter_t *, picture_t *);

static int getAnalyticType(int id);

void rotate_on_click_1X4(filter_t *p_filter, IplImage* input, IplImage* output, int x_old, int y_old, int x_diff, int y_diff);
void zoom_perspective(filter_t *p_filter, IplImage *output, IplImage *input, double radius, double phi, int position, double zoom);
void create_Perspective(filter_t *p_filter, IplImage* output, IplImage *input, double theta1, double theta2, int position);
void zoom_on_click(filter_t *, IplImage* input, IplImage* output, int point_x, int point_y);
static int Mouse(filter_t *, vlc_mouse_t *, const vlc_mouse_t *, const vlc_mouse_t *);
static int MouseWheelCallback(vlc_object_t *, char const *,
        vlc_value_t, vlc_value_t, void *);
double HOME_MADE_ATOI(string temp);
void savedata(filter_t *p_filter);
void savepreset(filter_sys_t *p_sys, char* preset_id);
void readpreset(filter_sys_t *p_sys, char* preset_id);



// To Save the preset values
void savepreset(filter_sys_t *p_sys, char* preset_id)
 {
    ofstream outf;
    outf.open(preset_id, std::ofstream::out | std::ofstream::trunc);
    if (!outf)
    {
        cerr << "Uh oh, Sample.dat could not be opened for writing!" << endl;
        return;
    }

    char para[50];


    char rotate_on_click_1X4_radius1[50];
    char rotate_on_click_1X4_radius2[50];
    char rotate_on_click_1X4_radius3[50];

    char rotate_on_click_1X4_phi1[50];
    char rotate_on_click_1X4_phi2[50];
    char rotate_on_click_1X4_phi3[50];

    char rotate_on_click_1X4_zoom1[50];
    char rotate_on_click_1X4_zoom2[50];
    char rotate_on_click_1X4_zoom3[50];


    char point_x_on_click_zoom[50];
    char point_y_on_click_zoom[50];
    char zoom_on_click_zoom[50];


    char double_view_angle_rotate[50];

    int n;
    n = sprintf(para, "%.4f", p_sys->para);
        ////////////////////////////////////////////////////////////////////////////
    n = sprintf(rotate_on_click_1X4_radius1, "%.4f", p_sys->radius1);
    n = sprintf(rotate_on_click_1X4_radius2, "%.4f", p_sys->radius2);
    n = sprintf(rotate_on_click_1X4_radius3, "%.4f", p_sys->radius3);

    n = sprintf(rotate_on_click_1X4_phi1, "%.4f", p_sys->phi1);
    n = sprintf(rotate_on_click_1X4_phi2, "%.4f", p_sys->phi2);
    n = sprintf(rotate_on_click_1X4_phi3, "%.4f", p_sys->phi3);

    n = sprintf(rotate_on_click_1X4_zoom1, "%.4f", p_sys->zoom_1X4_1);
    n = sprintf(rotate_on_click_1X4_zoom2, "%.4f", p_sys->zoom_1X4_2);
    n = sprintf(rotate_on_click_1X4_zoom3, "%.4f", p_sys->zoom_1X4_3);

    ////////////////////////////////////////////////////////////////////////////

    n = sprintf(point_x_on_click_zoom, "%.4f", p_sys->point_x_on_click_zoom);
    n = sprintf(point_y_on_click_zoom, "%.4f", p_sys->point_y_on_click_zoom);
    n = sprintf(zoom_on_click_zoom, "%.4f", p_sys->zoom_on_click_zoom);

    ////////////////////////////////////////////////////////////////////////////

    n = sprintf(double_view_angle_rotate, "%.4f", p_sys->angle_rotate);

    ////////////////////////////////////////////////////////////////////////////
    //  We'll write two lines into this file
   outf << "para = " << para<< endl;

   outf << "rotate_on_click_1X4_radius1 = " << rotate_on_click_1X4_radius1<< endl;
   outf << "rotate_on_click_1X4_radius2 = " << rotate_on_click_1X4_radius2<< endl;
   outf << "rotate_on_click_1X4_radius3 = " << rotate_on_click_1X4_radius3<< endl;

   outf << "rotate_on_click_1X4_phi1 = " << rotate_on_click_1X4_phi1<< endl;
   outf << "rotate_on_click_1X4_phi2 = " << rotate_on_click_1X4_phi2<< endl;
   outf << "rotate_on_click_1X4_phi3 = " << rotate_on_click_1X4_phi3<< endl;

   outf << "rotate_on_click_1X4_zoom1 = " << rotate_on_click_1X4_zoom1<< endl;
   outf << "rotate_on_click_1X4_zoom2 = " << rotate_on_click_1X4_zoom2<< endl;
   outf << "rotate_on_click_1X4_zoom3 = " << rotate_on_click_1X4_zoom3<< endl

   outf << "point_x_on_click_zoom = " << point_x_on_click_zoom<< endl;
   outf << "point_y_on_click_zoom = " << point_y_on_click_zoom<< endl;
   outf << "zoom_on_click_zoom = " << zoom_on_click_zoom<< endl;

   outf << "double_view_angle_rotate = " << double_view_angle_rotate<< endl;


outf.close();

}

// To Read the value of selected preset
 void readpreset(filter_sys_t *p_sys, char* preset_id)
{
    ifstream input;
    input.open(preset_id);
    if (!input)
    {
        cout << "camera1.txt could not be accessed!" << endl;
    }
    string temp;

    //String data from file, to be converted to an int
    //Get data from file

    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->para = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->radius1 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->radius2 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->radius3 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->phi1 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->phi2 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->phi3 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->zoom_1X4_1 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->zoom_1X4_2 = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->zoom_1X4_3 = HOME_MADE_ATOI(temp);

    }


    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->point_x_on_click_zoom = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->point_y_on_click_zoom = HOME_MADE_ATOI(temp);
    }
    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->zoom_on_click_zoom = HOME_MADE_ATOI(temp);
    }

    if (!input.eof())
    {
        getline(input, temp); //Get 1 line from the file and place into temp
        p_sys->angle_rotate = HOME_MADE_ATOI(temp);
    }

    input.close();
}

double HOME_MADE_ATOI(string temp)
{
    int k;
    for (k = 0; temp[k] != '\0'; k++)
    {
        if (temp[k] == '=')
            break;
    }

    k = k + 2;

    char str[20];
    int j = 0;

    for (k; temp[k] != '\0'; k++)
    {
        str[j] = temp[k];
        j++;
    }
    str[j] = '\0';

    int i, point_location;
    double output = 0.0;
    double decimal = 0.0;
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
        {
            point_location = i;
            break;
        }
    }
    for (i = 0; i < point_location; i++)
    {
        output = output * 10 + (float) (str[i] - '0');
    }
    for (i = strlen(str) - 1; i >= point_location + 1; i--)
    {
        decimal = decimal / 10 + (float) (str[i] - '0');
    }

    decimal /= 10;
    output = output + decimal;
    return output;
}

//To save data into file
void savedata(filter_t *p_filter)
{
    ofstream outf;
    outf.open(p_filter->p_sys->camera_id, std::ofstream::out | std::ofstream::trunc);
    if (!outf)
    {
        cerr << "Uh oh, Sample.dat could not be opened for writing!" << endl;
        return;
    }

    char radius[50];
    char theta[50];
    char centre_x[50];
    char centre_y[50];
    int n;
    n = sprintf(radius, "%.4f", p_filter->p_sys->radius_video);
    n = sprintf(theta, "%.4f", p_filter->p_sys->theta);
    n = sprintf(centre_x, "%.4f", p_filter->p_sys->centre_x);
    n = sprintf(centre_y, "%.4f", p_filter->p_sys->centre_y);

    //  We'll write two lines into this file
    outf << "radius   = " << radius << endl;
    outf << "theta    = " << theta << endl;
    outf << "centre_x = " << centre_x << endl;
    outf << "centre_y = " << centre_y << endl;
    outf.close();

}
/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
vlc_module_begin()
set_description(N_("OpenCV fisheye unwrapping filter"))
set_shortname(N_("OpenCV fisheye"))
set_capability("opencv fisheye", 1)
add_shortcut("opencv_fisheye")
set_category(CAT_VIDEO)
set_subcategory(SUBCAT_VIDEO_VFILTER2)
set_callbacks(OpenFilter, CloseFilter)

add_string("opencv-haarcascade-file", "c:\\haarcascade_frontalface_alt.xml", NULL,N_("Haar cascade filename"),N_("Name of XML file containing Haar cascade description"), false);

vlc_module_end()

/*****************************************************************************
 * OpenFilter: probe the filter and return score
 *****************************************************************************/
static int OpenFilter(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t*) p_this;
    filter_sys_t *p_sys;

    /* Allocate the memory needed to store the decoder's structure */
    if ((p_filter->p_sys = p_sys =
            (filter_sys_t *) malloc(sizeof (filter_sys_t))) == NULL) {
        return VLC_ENOMEM;
    }

    vlc_value_t val1;

    var_Get(p_filter->p_parent->p_parent, "video-object", &val1);

    analytic_input_params *parameters = (analytic_input_params*) val1.p_address;

    p_sys->input_parameters = parameters;

    p_sys->event_info.i_region_size = 0;
    p_sys->event_info.p_region = NULL;
    p_sys->i_id = 0;

    p_sys->counter = 0;

    p_filter->p_sys->x_old = 0;
    p_filter->p_sys->x_new = 0;
    p_filter->p_sys->y_old = 0;
    p_filter->p_sys->y_new = 0;
    p_filter->p_sys->mov = 0;
    p_filter->p_sys->click = 0;
    p_filter->p_sys->save_preset = 0;
    p_filter->p_sys->scroll_value = 0;
    p_filter->p_sys->Var_on_click_zoom = 0;
    p_filter->p_sys->angle_rotate = 0;
    p_filter->p_sys->start = 1;

    ///////////////////////////////////////////////////////text file assignment/////////////////////

    //////////////// creating folder  "fisheye_config" ////////////////////////


    char idnum[20];
    sprintf(idnum, "%d", p_filter->p_sys->input_parameters->cameraid);
    char name[] = "fisheye_config/camera_num_";
    strcpy(p_filter->p_sys->camera_id, name);
    char txt[] = ".txt";
    strcat(p_filter->p_sys->camera_id, idnum);
    strcat(p_filter->p_sys->camera_id, txt);

    //Open the input file
    ifstream input;
    input.open(p_filter->p_sys->camera_id);
    if (!input)
    {
        cout << "camera1.txt could not be accessed!" << endl;
        ofstream outf(p_filter->p_sys->camera_id);

        // If we couldn't open the output file stream for writing
        if (!outf)
        {
            // Print an error and exit
            cerr << "Uh oh, Sample.dat could not be opened for writing!" << endl;
            return 0;
        }

        // We'll write two lines into this file
        outf << "radius   = 400.00" << endl;
        outf << "theta    = 1.57" << endl;
        outf << "centre_x = 200.00" << endl;
        outf << "centre_y = 200.00" << endl;
        outf.close();
        p_filter->p_sys->start = 0;

        input.open(p_filter->p_sys->camera_id);
    }
    else
    {
        //Variables
        string temp;

        if (!input.eof())
        {
            getline(input, temp); //Get 1 line from the file and place into temp
            p_filter->p_sys->radius_video = HOME_MADE_ATOI(temp);
        }
        if (!input.eof())
        {
            getline(input, temp); //Get 1 line from the file and place into temp
            p_filter->p_sys->theta = HOME_MADE_ATOI(temp);
        }
        if (!input.eof())
        {
            getline(input, temp); //Get 1 line from the file and place into temp
            p_filter->p_sys->centre_x = HOME_MADE_ATOI(temp);
        }
        if (!input.eof())
        {
            getline(input, temp); //Get 1 line from the file and place into temp
            p_filter->p_sys->centre_y = HOME_MADE_ATOI(temp);
        }
    }

    input.close();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    p_filter->p_sys->para = p_filter->p_sys->input_parameters->debugParam.thresh4;
    p_filter->pf_video_filter = Filter;
    p_filter->pf_video_mouse = Mouse;
    ///////////////////////////////////////////////////////
    var_CreateGetIntegerCommand(p_filter, "mouse-wheel");
    var_AddCallback(p_filter, "mouse-wheel", MouseWheelCallback, p_sys);
    p_filter->p_sys->f_scale = p_filter->p_sys->input_parameters->debugParam.minObjHeight;
    p_filter->p_sys->output_max_height = p_filter->p_sys->input_parameters->debugParam.maxObjHeight;
    p_filter->p_sys->output_max_width = p_filter->p_sys->input_parameters->debugParam.maxObjWidth;
    ///////////////////////////////////////////////////////////////
    return VLC_SUCCESS;
}

/*****************************************************************************
 * CloseFilter: clean up the filter
 *****************************************************************************/
static void CloseFilter(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t*) p_this;
    filter_sys_t *p_sys = p_filter->p_sys;

    //////////////////////////////////////////////
    var_DelCallback(p_filter, "mouse-wheel", MouseWheelCallback, p_sys);
    ////////////////////////////////////////////////////////
    free(p_sys);

    msg_Dbg(p_filter, "===================In CloseFilter After psys");
}

/****************************************************************************
 * Filter: Check for faces and raises an event when one is found.
 ****************************************************************************
 * p_pic: A picture_t with its p_data_orig member set to an array of
 * IplImages (one image for each picture_t plane).
 ****************************************************************************/
static picture_t *Filter(filter_t *p_filter, picture_t *p_pic)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    IplImage** p_img = NULL;
    int i_planes = 0;
    int i, scale = 1;

    if ((!p_pic))
    {
        msg_Err(p_filter, "no image array");
        return NULL;
    }
    if (!(p_pic->p_data_orig))
    {
        msg_Err(p_filter, "no image array");
        return NULL;
    }

    p_img = (IplImage**) p_pic->p_data_orig;
    i_planes = p_pic->i_planes;


    //check the image array for validity
    if ((!p_img[0])) //1st plane is 'I' i.e. greyscale
    {
        msg_Err(p_filter, "no image");
        return NULL;
    }
    p_sys->counter++;

    if (i_planes < 1)
    {
        msg_Err(p_filter, "no image planes");
        return NULL;
    }


    IplImage* output;
    output = cvCloneImage(p_img[0]);


    p_filter->p_sys->diff_x = p_filter->p_sys->x_new - p_filter->p_sys->x_old;
    p_filter->p_sys->diff_y = p_filter->p_sys->y_new - p_filter->p_sys->y_old;

    ///////////////////////////////////////////////////////////
    if (p_sys->counter == 1)
    {
        p_filter->p_sys->radius_video_check = output->height / 2;
        p_filter->p_sys->radius_video_temp = output->height / 2;
        p_filter->p_sys->r = output->height / 2;
        p_filter->p_sys->radius1 = p_filter->p_sys->r / 2;
        p_filter->p_sys->phi1 = 3.14 / 3;
        p_filter->p_sys->radius2 = p_filter->p_sys->r / 2;
        p_filter->p_sys->phi2 = 3 * 3.14 / 4;
        p_filter->p_sys->radius3 = p_filter->p_sys->r / 2;
        p_filter->p_sys->phi3 = -3.14 / 3;
        p_filter->p_sys->diff_x = 0;
        p_filter->p_sys->diff_y = 0;
        p_filter->p_sys->zoom_1X4_1 = 1;
        p_filter->p_sys->zoom_1X4_2 = 1;
        p_filter->p_sys->zoom_1X4_3 = 1;
        p_filter->p_sys->zoom_on_click_zoom = 1;
        p_filter->p_sys->centre_x = output->width / 2;
        p_filter->p_sys->centre_y = output->height / 2;
        p_filter->p_sys->minimum = output->height / 2;
        p_filter->p_sys->output_height = output->height;
        p_filter->p_sys->output_width = output->width;
        p_filter->p_sys->point_x_on_click_zoom = p_filter->p_sys->r / 2;
        p_filter->p_sys->point_y_on_click_zoom = 0;
        p_filter->p_sys->on_click_radius = p_filter->p_sys->r / 2;
        p_filter->p_sys->on_click_theta = 0;

    }



    if (p_filter->p_sys->start == 0)
    {
        p_filter->p_sys->para = 0;

        int output_i, output_j, input_i, input_j;

        for (output_i = 0; output_i < (int) (p_filter->p_sys->output_max_height); output_i++)
        {
            for (output_j = 0; output_j < (int) (p_filter->p_sys->output_max_width); output_j++)
            {
                input_i = output_i * p_filter->p_sys->f_scale;
                input_j = output_j * p_filter->p_sys->f_scale;

                p_img[0]->imageData[output->widthStep * output_i + output_j * 3] = output->imageData[output->widthStep * input_i + input_j * 3];
                p_img[0]->imageData[output->widthStep * output_i + output_j * 3 + 1] = output->imageData[output->widthStep * input_i + input_j * 3 + 1];
                p_img[0]->imageData[output->widthStep * output_i + output_j * 3 + 2] = output->imageData[output->widthStep * input_i + input_j * 3 + 2];
            }
        }

        int temp_x, temp_y;
        temp_x = p_filter->p_sys->point_x;
        temp_y = p_filter->p_sys->point_y;
        if (temp_x >= p_filter->p_sys->output_height / 16 && temp_x <= p_filter->p_sys->output_width - p_filter->p_sys->output_height / 16)
        {
            p_filter->p_sys->centre_x = temp_x;
        }
        if (temp_y >= p_filter->p_sys->output_height / 16 && temp_y <= p_filter->p_sys->output_height - p_filter->p_sys->output_height / 16)
        {
            p_filter->p_sys->centre_y = temp_y;
        }

        double horizontal_lower, horizontal_upper, vertical_lower, vertical_upper;

        horizontal_lower = p_filter->p_sys->centre_x;
        horizontal_upper = p_filter->p_sys->output_width - p_filter->p_sys->centre_x;
        vertical_lower = p_filter->p_sys->output_height - p_filter->p_sys->centre_y;
        vertical_upper = p_filter->p_sys->centre_y;

        p_filter->p_sys->minimum = min(horizontal_lower, horizontal_upper);
        p_filter->p_sys->minimum = min(p_filter->p_sys->minimum, vertical_lower);
        p_filter->p_sys->minimum = min(p_filter->p_sys->minimum, vertical_upper);


        p_filter->p_sys->radius_video_temp = min(p_filter->p_sys->radius_video_check, p_filter->p_sys->minimum);

        cvCircle(p_img[0], cvPoint((p_filter->p_sys->centre_x / p_filter->p_sys->f_scale),(p_filter->p_sys->centre_y / p_filter->p_sys->f_scale)), p_filter->p_sys->radius_video_temp / p_filter->p_sys->f_scale, CV_RGB(0, 255, 0), 2);
    }

    //Calling rotate_on_click_1X4
    if (p_filter->p_sys->para == 1) rotate_on_click_1X4(p_filter, output, p_img[0], p_filter->p_sys->x_old, p_filter->p_sys->y_old, p_filter->p_sys->diff_x, p_filter->p_sys->diff_y);

    //Calling zoom_on_click
    if (p_filter->p_sys->para == 2)
    {
        if (p_filter->p_sys->Var_on_click_zoom == 1)
        {
            p_filter->p_sys->point_x_on_click_zoom = p_filter->p_sys->point_x;
            p_filter->p_sys->point_y_on_click_zoom = p_filter->p_sys->point_y;
        }
        zoom_on_click(p_filter, output, p_img[0], p_filter->p_sys->point_x_on_click_zoom, p_filter->p_sys->point_y_on_click_zoom);
    }

    //Calling create_Perspective
    if (p_filter->p_sys->para == 3)
    {
        p_filter->p_sys->angle_rotate = p_filter->p_sys->angle_rotate + (double) p_filter->p_sys->diff_x / 3000;

        if (p_filter->p_sys->angle_rotate > 2 * 3.14) p_filter->p_sys->angle_rotate = 0;
        if (p_filter->p_sys->angle_rotate < -2 * 3.14) p_filter->p_sys->angle_rotate = 0;

        create_Perspective(p_filter, p_img[0], output, p_filter->p_sys->angle_rotate, 3.14 + p_filter->p_sys->angle_rotate, 1);
        create_Perspective(p_filter, p_img[0], output, 3.14 + p_filter->p_sys->angle_rotate, 2 * 3.14 + p_filter->p_sys->angle_rotate, 2);

    }

    cvReleaseImage(&output);

    return p_pic;
}

void rotate_on_click_1X4(filter_t *p_filter, IplImage* input, IplImage* output, int x_old, int y_old, int x_diff, int y_diff)
{
    int output_i, output_j, input_i, input_j;

    if (y_old < input->height / 2 && x_old > input->width / 2)
    {
        p_filter->p_sys->radius1 = p_filter->p_sys->radius1 - double(y_diff) / 7;

        if (p_filter->p_sys->radius1 > p_filter->p_sys->radius_video)
            p_filter->p_sys->radius1 = p_filter->p_sys->radius_video;
        else if (p_filter->p_sys->radius1 < 0)
            p_filter->p_sys->radius1 = 0;

        p_filter->p_sys->phi1 = p_filter->p_sys->phi1 - double(x_diff) / 3000;

    }
    else if (y_old > input->height / 2 && x_old < input->width / 2)
    {
        p_filter->p_sys->radius2 = p_filter->p_sys->radius2 - double(y_diff) / 7;

        if (p_filter->p_sys->radius2 > p_filter->p_sys->radius_video)
            p_filter->p_sys->radius2 = p_filter->p_sys->radius_video;
        else if (p_filter->p_sys->radius2 < 0)
            p_filter->p_sys->radius2 = 0;

        p_filter->p_sys->phi2 = p_filter->p_sys->phi2 - double(x_diff) / 3000;
    }
    else if (y_old > input->height / 2 && x_old > input->width / 2)
    {
        p_filter->p_sys->radius3 = p_filter->p_sys->radius3 - double(y_diff) / 7;

        if (p_filter->p_sys->radius3 > p_filter->p_sys->radius_video)
            p_filter->p_sys->radius3 = p_filter->p_sys->radius_video;
        else if (p_filter->p_sys->radius3 < 0)
            p_filter->p_sys->radius3 = 0;

        p_filter->p_sys->phi3 = p_filter->p_sys->phi3 - double(x_diff) / 3000;
    }

    zoom_perspective(p_filter, output, input, p_filter->p_sys->radius1, p_filter->p_sys->phi1, 2, p_filter->p_sys->zoom_1X4_1);
    zoom_perspective(p_filter, output, input, p_filter->p_sys->radius2, p_filter->p_sys->phi2, 3, p_filter->p_sys->zoom_1X4_2);
    zoom_perspective(p_filter, output, input, p_filter->p_sys->radius3, p_filter->p_sys->phi3, 4, p_filter->p_sys->zoom_1X4_3);

    // INPUT ON ONE SIDE
    int width_max = p_filter->p_sys->output_max_width;
    int height_max = p_filter->p_sys->output_max_height;
    for (output_j = 0; output_j < width_max / 2; output_j++)
    {
        for (output_i = 0; output_i < height_max / 2; output_i++)
        {
            input_i = 2 * output_i*p_filter->p_sys->f_scale;
            input_j = 2 * output_j*p_filter->p_sys->f_scale;

            output->imageData[output->widthStep * output_i + output_j * 3] = input->imageData[input->widthStep * input_i + input_j * 3];
            output->imageData[output->widthStep * output_i + output_j * 3 + 1] = input->imageData[input->widthStep * input_i + input_j * 3 + 1];
            output->imageData[output->widthStep * output_i + output_j * 3 + 2] = input->imageData[input->widthStep * input_i + input_j * 3 + 2];
        }
    }
}

void zoom_perspective(filter_t *p_filter, IplImage *output, IplImage *input, double radius, double phi, int position, double zoom)
{
    double zoom_radius, radius_max, radius_min;
    int p, q;
    int output_i, output_j, input_i, input_j;

    if (position == 2)
    {
        p = 0;
        q = 1;
    }
    else if (position == 3)
    {
        p = 1;
        q = 0;
    }
    else if (position == 4)
    {
        p = 1;
        q = 1;
    }

    zoom_radius = p_filter->p_sys->radius_video / (2 * zoom);

    if (radius * radius > p_filter->p_sys->radius_video * p_filter->p_sys->radius_video)
    {
        printf("Invalid Selection\n");
    }
    else
    {
        if (radius <= zoom_radius)
        {
            radius_max = 2 * zoom_radius;
            radius_min = 0;
        }
        else if (p_filter->p_sys->radius_video - radius <= zoom_radius)
        {
            radius_max = p_filter->p_sys->radius_video;
            radius_min = p_filter->p_sys->radius_video - 2 * zoom_radius;
        }
        else
        {
            radius_max = radius + zoom_radius;
            radius_min = radius - zoom_radius;
        }
        double temp_x, temp_y, new_x, new_y, width_min, width_max;

        for (output_j = 0; output_j < p_filter->p_sys->output_max_width / 2; output_j++)
        {
            temp_y = zoom_radius - (2 * zoom_radius * double(output_j)) / (double(p_filter->p_sys->output_max_width) / 2);
            width_min = radius_min;
            width_max = sqrt(radius_max * radius_max - temp_y * temp_y);

            for (output_i = 0; output_i < p_filter->p_sys->output_max_height / 2; output_i++)
            {
                temp_x = width_max - double(output_i)*(width_max - width_min) / double(p_filter->p_sys->output_max_height / 2);
                new_x = temp_x * cos(-phi) + temp_y * sin(-phi);
                new_y = -temp_x * sin(-phi) + temp_y * cos(-phi);
                input_i = (int) p_filter->p_sys->centre_y - (int) (round(new_y));
                input_j = (int) p_filter->p_sys->centre_x + (int) (round(new_x));

                output->imageData[output->widthStep * (output_i + p * (int) (p_filter->p_sys->output_max_height) / 2) + (output_j + q * (int) (p_filter->p_sys->output_max_width) / 2) * 3] = input->imageData[input->widthStep * input_i + input_j * 3];
                output->imageData[output->widthStep * (output_i + p * (int) (p_filter->p_sys->output_max_height) / 2) + (output_j + q * (int) (p_filter->p_sys->output_max_width) / 2) * 3 + 1] = input->imageData[input->widthStep * input_i + input_j * 3 + 1];
                output->imageData[output->widthStep * (output_i + p * (int) (p_filter->p_sys->output_max_height) / 2) + (output_j + q * (int) (p_filter->p_sys->output_max_width) / 2) * 3 + 2] = input->imageData[input->widthStep * input_i + input_j * 3 + 2];
            }
        }
    }
}

void create_Perspective(filter_t *p_filter, IplImage *output, IplImage *input, double theta1, double theta2, int position)
{
    int p;
    if (position == 1)
    {
        p = 0;
    }
    else if (position == 2)
    {
        p = 1;
    }

    int inputX, inputY, outputX, outputY;
    double radius, theta, tempX, tempY;
    int max_width = p_filter->p_sys->output_max_width;
    int max_height = p_filter->p_sys->output_max_height / 2;


    for (outputX = 0; outputX < (int) max_height; outputX++)
    {
        radius = (double) (p_filter->p_sys->radius_video - 4 * p_filter->p_sys->radius_video * outputX / (5 * max_height));

        for (outputY = 0; outputY < (int) max_width; outputY++)
        {
            theta = (double) (theta2 + outputY * (theta1 - theta2) / (max_width));
            tempX = radius * sin(theta);
            tempY = radius * cos(theta);
            inputX = (int) p_filter->p_sys->centre_y - (int) (round(tempX));
            inputY = (int) p_filter->p_sys->centre_x + (int) (round(tempY));
            output->imageData[output->widthStep * (outputX + p * (int) (max_height)) + outputY * 3] = input->imageData[input->widthStep * inputX + inputY * 3];
            output->imageData[output->widthStep * (outputX + p * (int) (max_height)) + outputY * 3 + 1] = input->imageData[input->widthStep * inputX + inputY * 3 + 1];
            output->imageData[output->widthStep * (outputX + p * (int) (max_height)) + outputY * 3 + 2] = input->imageData[input->widthStep * inputX + inputY * 3 + 2];
        }
    }
}

void zoom_on_click(filter_t *p_filter, IplImage* input, IplImage* output, int point_x, int point_y)
{
    double zoom_radius, radius_max, radius_min, radius, phi, p, q;
    int output_i, output_j, input_i, input_j;

    IplImage* output2;
    output2 = cvCloneImage(output);

    p = (point_x)*3 - (int) p_filter->p_sys->centre_x;
    q = (int) p_filter->p_sys->centre_y - (point_y)*3;
    radius = sqrt(p * p + q * q);


    //FINDING THE QUADRANT OF POINT TO CALCULATE ITS ANGLE

    if (p == 0 && q >= 0)
    {
        phi = 3.14 / 2;
    }
    else if (p == 0 && q < 0)
    {
        phi = 3 * 3.14 / 2;
    }
    else if(q == 0 && p > 0)
    {
        phi = 0;
    }
    else if(q == 0 && p < 0)
    {
        phi = 3.14;
    }
    else if (p > 0 && q > 0)
    {
        phi = atan(q / p);
    }
    else if (p < 0 && q > 0)
    {
        phi = 3.14 - atan(abs(q / p));
    }
    else if (p < 0 && q < 0)
    {
        phi = 3.14 + atan(q / p);
    }
    else if (p > 0 && q < 0)
    {
        phi = 2 * 3.14 - atan(abs(q / p));
    }


    zoom_radius = p_filter->p_sys->radius_video / (2 * p_filter->p_sys->zoom_on_click_zoom);

    if (radius * radius > p_filter->p_sys->radius_video * p_filter->p_sys->radius_video)
    {
        printf("Invalid Selection\n");
    }
    else
    {
        p_filter->p_sys->on_click_radius = radius;
        p_filter->p_sys->on_click_theta = phi;
    }

    if (p_filter->p_sys->on_click_radius <= zoom_radius)
    {
        radius_max = 2 * zoom_radius;
        radius_min = 0;
    }
    else if (p_filter->p_sys->radius_video - p_filter->p_sys->on_click_radius <= zoom_radius)
    {
        radius_max = p_filter->p_sys->radius_video;
        radius_min = p_filter->p_sys->radius_video - 2 * zoom_radius;
    } else
    {
        radius_max = p_filter->p_sys->on_click_radius + zoom_radius;
        radius_min = p_filter->p_sys->on_click_radius - zoom_radius;
    }
    double temp_x, temp_y, new_x, new_y, width_min, width_max;
    for (output_j = 0; output_j < p_filter->p_sys->output_max_width; output_j++)
    {
        temp_y = zoom_radius - (2 * zoom_radius * double(output_j)) / (double(p_filter->p_sys->output_max_width));

        width_min = radius_min;

        width_max = sqrt(radius_max * radius_max - temp_y * temp_y);

        for (output_i = 0; output_i < p_filter->p_sys->output_max_height; output_i++)
        {
            temp_x = width_max - double(output_i)*(width_max - width_min) / double(p_filter->p_sys->output_max_height);
            new_x = temp_x * cos(-p_filter->p_sys->on_click_theta) + temp_y * sin(-p_filter->p_sys->on_click_theta);
            new_y = -temp_x * sin(-p_filter->p_sys->on_click_theta) + temp_y * cos(-p_filter->p_sys->on_click_theta);
            input_i = (int) p_filter->p_sys->centre_y - (int) (round(new_y));
            input_j = (int) p_filter->p_sys->centre_x + (int) (round(new_x));


            output->imageData[output->widthStep * output_i + (output_j) * 3] = input->imageData[input->widthStep * input_i + input_j * 3];
            output->imageData[output->widthStep * output_i + (output_j) * 3 + 1] = input->imageData[input->widthStep * input_i + input_j * 3 + 1];
            output->imageData[output->widthStep * output_i + (output_j) * 3 + 2] = input->imageData[input->widthStep * input_i + input_j * 3 + 2];
            output2->imageData[output2->widthStep * input_i + input_j * 3 + 2] = 255;
        }
    }

    for (output_i = 0; output_i < (int) (p_filter->p_sys->output_max_height / 3); output_i++)
    {
        for (output_j = 0; output_j < (int) (p_filter->p_sys->output_max_width / 3); output_j++)
        {
            input_i = 3 * output_i * p_filter->p_sys->f_scale;
            input_j = 3 * output_j * p_filter->p_sys->f_scale;

            output->imageData[output->widthStep * output_i + output_j * 3] = output2->imageData[output2->widthStep * input_i + input_j * 3];
            output->imageData[output->widthStep * output_i + output_j * 3 + 1] = output2->imageData[output2->widthStep * input_i + input_j * 3 + 1];
            output->imageData[output->widthStep * output_i + output_j * 3 + 2] = output2->imageData[output2->widthStep * input_i + input_j * 3 + 2];

        }
    }


    cvReleaseImage(&output2);
}

static int Mouse(filter_t *p_filter, vlc_mouse_t *p_mouse, const vlc_mouse_t *p_old, const vlc_mouse_t *p_new)
{
    static bool mousemov;
    static bool hasPressed;
    static bool isLeftreleased;
    static bool isLeftPressed;

    p_filter->p_sys->point_x = p_new->i_x * p_filter->p_sys->f_scale;
    p_filter->p_sys->point_y = p_new->i_y * p_filter->p_sys->f_scale;
    hasPressed = vlc_mouse_HasPressed(p_old, p_new, MOUSE_BUTTON_LEFT);
    isLeftPressed = vlc_mouse_IsLeftPressed(p_new);
    mousemov = vlc_mouse_HasMoved(p_old, p_new);
    isLeftreleased = vlc_mouse_HasReleased(p_old, p_new, MOUSE_BUTTON_LEFT);


    if (isLeftPressed)
    {
        p_filter->p_sys->x_new = p_filter->p_sys->point_x;
        p_filter->p_sys->y_new = p_filter->p_sys->point_y;
    }
    if (hasPressed)
    {
        if (p_filter->p_sys->start == 0)
        {
            p_filter->p_sys->para = p_filter->p_sys->input_parameters->debugParam.thresh4;
            p_filter->p_sys->start = 1;
            p_filter->p_sys->radius_video = p_filter->p_sys->radius_video_temp-1;

            savedata(p_filter);
        }

        p_filter->p_sys->click = 1;

        p_filter->p_sys->x_old = p_filter->p_sys->point_x;
        p_filter->p_sys->y_old = p_filter->p_sys->point_y;
        p_filter->p_sys->Var_on_click_zoom = 1;
    }
    else if (mousemov)
    {
        p_filter->p_sys->mov = 1;
    }
    else if (isLeftreleased)
    {
        p_filter->p_sys->x_new = 0;
        p_filter->p_sys->y_new = 0;
        p_filter->p_sys->x_old = 0;
        p_filter->p_sys->y_old = 0;
        p_filter->p_sys->mov = 0;
        p_filter->p_sys->click = 0;
        p_filter->p_sys->Var_on_click_zoom = 0;

    }

    if (p_filter->p_sys->click == 1 && p_filter->p_sys->mov == 1)
    {
        p_filter->p_sys->x_new = p_filter->p_sys->point_x;
        p_filter->p_sys->y_new = p_filter->p_sys->point_y;
    }

    return VLC_SUCCESS;
}

static int MouseWheelCallback(vlc_object_t *p_this, char const *psz_var,vlc_value_t oldval, vlc_value_t newval,void *p_data)
{
    VLC_UNUSED(p_this);
    VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
    if (newval.i_int == 11)
    {
        p_sys->scroll_value = 10;
    }
    if (newval.i_int == 12)
    {
        p_sys->scroll_value = -10;
    }
    if (newval.i_int == 14)
    {
        p_sys->para = 1;
    }
    //Quad View
    if (newval.i_int == 15)
    {
        p_sys->para = 2;
    }
    //Click View
    if (newval.i_int == 16)
    {
        p_sys->para = 3;
    }
    //Double Prespective View

    if (newval.i_int == 13)
    {
        p_sys->start = 0;
    }
    if(newval.i_int == 17)
    {
        char presetnum[20];
        char preset_id[50];
        int presetid = 1;
        sprintf(presetnum, "%d", presetid);
        char name[] = "fisheye_config/preset_num_";
        strcpy(preset_id, name);
        char txt[] = ".txt";
        strcat(preset_id, presetnum);
        strcat(preset_id, txt);
        savepreset(p_sys,preset_id);
     }

     if(newval.i_int == 18)
     {
        char presetnum[20];
        char preset_id[50];
        int presetid = 1;
        sprintf(presetnum, "%d", presetid);
        char name[] = "fisheye_config/preset_num_";
        strcpy(preset_id, name);
        char txt[] = ".txt";
        strcat(preset_id, presetnum);
        strcat(preset_id, txt);
        readpreset(p_sys,preset_id);
     }
    if (p_sys->start == 0)
    {
        p_sys->radius_video = p_sys->radius_video + p_sys->scroll_value / 2;
        if (p_sys->radius_video < p_sys->output_height / 8) p_sys->radius_video = p_sys->output_height / 8;
        if (p_sys->radius_video > (p_sys->output_height / 2)) p_sys->radius_video = (p_sys->output_height / 2);
        p_sys->radius_video_check = p_sys->radius_video;

        return VLC_SUCCESS;
    }

    //para 1 for rotate on curl
    //para 2 for zoom on click
    //para 3 for create perspective

    if (p_sys->para == 2)
    {
        if ((p_sys->zoom_on_click_zoom + p_sys->scroll_value / 200 <= 3)&&(p_sys->zoom_on_click_zoom + p_sys->scroll_value / 200 >= 1))
            p_sys->zoom_on_click_zoom = p_sys->zoom_on_click_zoom + p_sys->scroll_value / 200;
    }
    if (p_sys->para == 1)
    {

        if (p_sys->point_y < p_sys->output_height / 2 && p_sys->point_x > p_sys->output_width / 2)
        {
            if ((p_sys->zoom_1X4_1 + p_sys->scroll_value / 200 <= 3)&&(p_sys->zoom_1X4_1 + p_sys->scroll_value / 200 >= 1))
                p_sys->zoom_1X4_1 += p_sys->scroll_value / 200;

        }
        else if (p_sys->point_y > p_sys->output_height / 2 && p_sys->point_x < p_sys->output_width / 2)
        {
            if ((p_sys->zoom_1X4_2 + p_sys->scroll_value / 200 <= 3)&&(p_sys->zoom_1X4_2 + p_sys->scroll_value / 200 >= 1))
                p_sys->zoom_1X4_2 += p_sys->scroll_value / 200;
        }
        else if (p_sys->point_y > p_sys->output_height / 2 && p_sys->point_x > p_sys->output_width / 2)
        {
            if ((p_sys->zoom_1X4_3 + p_sys->scroll_value / 200 <= 3)&&(p_sys->zoom_1X4_3 + p_sys->scroll_value / 200 >= 1))
                p_sys->zoom_1X4_3 += p_sys->scroll_value / 200;
        }
    }

    return VLC_SUCCESS;
}

///////////////////////////////////////////MODIFIED//////////////////////////////////////////////////////////
