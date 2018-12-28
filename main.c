/*=================================================================
 *project: DidItChange
 *
 *Description:     create a clickable pop up whenever a site has changed
 *
 *Author: Thomas Van Hoof
 *=================================================================
 *pseudo code for the project:
 *for all urls in argument of program
 *  download content of html of url into buffer.html
 *  compare content content of buffer.html to previously saved file*
 *  if different
 *     store into buffer
 *  else
 *    do nothing
 *send buffer to other program to visualize
 *=================================================================
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "curl/curl.h"
#include "gtk/gtk.h"

typedef struct changed_sites{
                struct changed_sites *px_next;
                char *pc_name;
                char *pc_site;
}Changed_sites;

/*@brief remove dynamically allocated linked list
 *
 *@param    px_site    pointer to Changed_site to remove
 */
static void v_freeAll(Changed_sites *px_site)
{
  Changed_sites *px_tmp;

  while( NULL != px_site)
  {
    px_tmp = px_site;
    free(px_site->pc_name);
    free(px_site->pc_site);
    px_site = px_site->px_next;
    free(px_tmp);
  }
}/*v_freeAll*/

/*@brief add site to linked list of cahnged sites
 *
 *@param ppx_sites    head of the linked list
 *@param pc_string    url to add to the linked list
 */
static void v_createLinkedList(Changed_sites **ppx_sites, char *pc_name, char *pc_string)
{
    if(NULL == *ppx_sites)
    {
        *ppx_sites = malloc(sizeof(Changed_sites));
        if(NULL != *ppx_sites)
        {
            (*ppx_sites)->px_next = NULL;
            (*ppx_sites)->pc_site = malloc(strlen(pc_string));
            if( NULL != (*ppx_sites)->pc_site )
            {
                strcpy((*ppx_sites)->pc_site, pc_string);
            }
            (*ppx_sites)->pc_name = malloc(strlen(pc_name));
            if( NULL != (*ppx_sites)->pc_name )
            {
                strcpy((*ppx_sites)->pc_name, pc_name);
            }
        }/*if(NULL != *ppx_sites)*/
    }/*if(NULL == *ppx_sites)*/
    else
    {
        Changed_sites *px_sites = *ppx_sites;
        for(;px_sites->px_next!=NULL;px_sites = px_sites->px_next);
        px_sites->px_next = malloc(sizeof(Changed_sites));
        if(NULL != px_sites->px_next)
        {
            px_sites->px_next->px_next = NULL;
            (*ppx_sites)->px_next->pc_site = malloc(strlen(pc_string));
            if( NULL != (*ppx_sites)->px_next->pc_site )
            {
                strcpy((*ppx_sites)->px_next->pc_site, pc_string);
            }
            (*ppx_sites)->px_next->pc_name = malloc(strlen(pc_name));
            if( NULL != (*ppx_sites)->px_next->pc_name )
            {
                strcpy((*ppx_sites)->px_next->pc_name, pc_name);
            }
        }/*if(NULL != px_sites->px_next)*/
    }/*else -- if(NULL == *ppx_sites)*/
}/*v_createLinkedList*/

/*@brief libcurl callback for downloading html to file
 *
 *@param pv_data    downloaded content
 *@param s_Size        size of doznloaded content
 *@param s_nmemb
 *@param pv_stream    filepointer to write data to*/
static size_t s_write_data(void *pv_data, size_t s_size, size_t s_nmemb, void *pv_stream)
{
    return fwrite(pv_data, s_size, s_nmemb, (FILE*)pv_stream);
}/*s_write_data*/

/*@brief let the string stop at the newline character
 *
 *@param    pc_string    this string will be ended where the \n character is
 */
static void v_breakAtNewline(char *pc_string)
{
    int i_i = 0;
    for(; i_i < strlen(pc_string); ++i_i)
    {
        if('\n' == *(pc_string + i_i))
        {
            *(pc_string + i_i) = '\0';
            break;
        }/*if('\n' == *(pc_string + i))*/
    }/*for(int i=0; i < strlen(pc_string); ++i)*/
}/*v_breakAtNewline*/

/*@brief compares the content of 2 files
 *
 *@param    fp_file1    FILE pointer to file to compare
 *@param    fp_file2    FILE pointer to file to compare
 *
 *@return true if files are identical, false otherwise
 *@note please make sure that both files can be read
 */
static int i_compareFiles(FILE* fp_file1, FILE *fp_file2)
{
    int i_ch1, i_ch2;
    //int i_ch;
    fseek(fp_file1, 0, SEEK_SET);
    fseek(fp_file2, 0, SEEK_SET);

    do
    {
        i_ch1 = getc(fp_file1);
        i_ch2 = getc(fp_file2);
    }while((EOF != i_ch1) && (EOF != i_ch2) && (i_ch1 == i_ch2));

    return i_ch1 == i_ch2;
}/*b_compareFiles*/

/*@brief generate a filename for a url
 *
 *@param    pc_filename        the generated filename
 *@param    pc_url                the given url to generate a filename for
 */
static void v_generateFileNameForUrl(char *pc_filename, char *pc_name)
{
    strcpy(pc_filename, pc_name);
    strcpy(&pc_filename[strlen(pc_filename)], ".html");
}/*v_generateFileNameForUrl*/

static void v_buttonClicked(GtkWidget *px_widget, gpointer data)
{
    Changed_sites *px_site = (Changed_sites *)data;
    char pc_filename[512] = {0};
    CURL *px_curlHandle;

    gtk_show_uri_on_window(NULL , px_site->pc_site, GDK_CURRENT_TIME, NULL);

    v_generateFileNameForUrl(pc_filename, px_site->pc_name);
    FILE *pf_output = fopen(pc_filename, "w+");

    /*initialize px_curlHandle*/
    //curl_global_init(CURL_GLOBAL_ALL);
    px_curlHandle = curl_easy_init();
    
    /*set the needed settings for px_curlHandle*/
    curl_easy_setopt(px_curlHandle, CURLOPT_VERBOSE, 0L); /*display all debug prints*/
    curl_easy_setopt(px_curlHandle, CURLOPT_NOPROGRESS, 1L); /*disable terminal statusbar*/
    curl_easy_setopt(px_curlHandle, CURLOPT_WRITEFUNCTION, s_write_data);    /*set callback*/
    curl_easy_setopt(px_curlHandle, CURLOPT_WRITEDATA, pf_output);    /*set the buffer file to download into*/
    curl_easy_setopt(px_curlHandle, CURLOPT_URL, px_site->pc_site);    /*set the url to download*/
    curl_easy_setopt(px_curlHandle, CURLOPT_TIMEOUT, 120);    /*time out after 2 minutes*/
    CURLcode x_curlcode = curl_easy_perform(px_curlHandle);
    switch(x_curlcode)
    {
        case CURLE_COULDNT_CONNECT:
        case CURLE_COULDNT_RESOLVE_HOST:
        case CURLE_COULDNT_RESOLVE_PROXY:
            fprintf(stderr, "request failed; %s\n", curl_easy_strerror(x_curlcode));
            exit(EXIT_FAILURE);
        default:
            break;
    }/*switch(x_curlcode)*/
}/*v_buttonClicked*/

static void v_createGUI(GtkApplication  *x_app, gpointer x_data)
{
    GtkWidget *px_window = gtk_application_window_new(x_app);
    GtkWidget *px_verticalContainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    Changed_sites *px_sites = (Changed_sites*)x_data;

    gtk_window_set_title(GTK_WINDOW(px_window), "DidItChange");
    //gtk_window_set_default_size(GTK_WINDOW(px_window), 200,200);

    while(NULL != px_sites)
    {
        GtkWidget *px_horizontalContainer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        GtkWidget *px_label;
        GtkWidget *px_button;

        px_label = gtk_label_new(px_sites->pc_name);
        px_button = gtk_button_new_with_label("visit");
        g_signal_connect(px_button, "clicked", G_CALLBACK(v_buttonClicked), px_sites);

        gtk_box_pack_start(GTK_BOX(px_horizontalContainer), px_label, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(px_horizontalContainer), px_button, FALSE, FALSE, 0);

        gtk_container_add(GTK_CONTAINER(px_verticalContainer), px_horizontalContainer);
        px_sites = px_sites->px_next;
    }/*while(NULL != px_sites)*/

    gtk_container_add(GTK_CONTAINER(px_window), px_verticalContainer);
    gtk_widget_show_all(px_window);
}/*v_createGUI*/

int main(int argc, char **argv)
{
    if(argc<2)
    {
        fprintf(stderr, "please give a filename as argument\n");
        exit(EXIT_FAILURE);
    }/*if(argc<2)*/
    FILE *pf_urls = fopen(argv[1], "r");
    char *pc_inputLine = NULL;
    size_t s_len;
    CURL *px_curlHandle;
    Changed_sites *px_sites = NULL;

    if(NULL == pf_urls)
    {
        fprintf(stderr, "file can not be opened\n");
        exit(EXIT_FAILURE);
    }/*if(NULL == pf_urls)*/


    /*initialize px_curlHandle*/
    curl_global_init(CURL_GLOBAL_ALL);
    px_curlHandle = curl_easy_init();
    
    /*set the needed settings for px_curlHandle*/
    curl_easy_setopt(px_curlHandle, CURLOPT_VERBOSE, 0L); /*display all debug prints*/
    curl_easy_setopt(px_curlHandle, CURLOPT_NOPROGRESS, 1L); /*disable terminal statusbar*/
    curl_easy_setopt(px_curlHandle, CURLOPT_WRITEFUNCTION, s_write_data);    /*set callback*/

    while(-1 != getline(&pc_inputLine, &s_len, pf_urls))
    {
        v_breakAtNewline(pc_inputLine);    /*end string at \n*/
        char *pc_name = strtok(pc_inputLine, ",");
        char *pc_url = strtok(NULL, "\n");
        FILE *pf_buffer = fopen("buffer.html", "w+");
        if(NULL != pf_buffer)
        {
            curl_easy_setopt(px_curlHandle, CURLOPT_WRITEDATA, pf_buffer);    /*set the buffer file to download into*/
            curl_easy_setopt(px_curlHandle, CURLOPT_URL, pc_url);    /*set the url to download*/
            curl_easy_setopt(px_curlHandle, CURLOPT_TIMEOUT, 120);    /*time out after 2 minutes*/
            CURLcode x_curlcode = curl_easy_perform(px_curlHandle);
            switch(x_curlcode)
            {
                case CURLE_COULDNT_CONNECT:
                case CURLE_COULDNT_RESOLVE_HOST:
                case CURLE_COULDNT_RESOLVE_PROXY:
                    fprintf(stderr, "request failed; %s\n", curl_easy_strerror(x_curlcode));
                    exit(EXIT_FAILURE);
                default:
                    break;
            }/*switch(x_curlcode)*/
            
            char pc_filename[256] = {0};
            v_generateFileNameForUrl(pc_filename, pc_name);
            FILE *pf_original = fopen(pc_filename, "r");
            if(NULL == pf_original)                                 /*file does not exist yet, so it has just been added*/
            {
                fclose(pf_buffer);
                rename("buffer.html", pc_filename);
            }/*if(NULL == pf_original)*/
            else if(0 == i_compareFiles(pf_buffer,pf_original))     /*file does exist and has changed*/
            {
                v_createLinkedList(&px_sites, pc_name, pc_url);
                //rename("buffer.html", pc_filename);
                fclose(pf_original);
                fclose(pf_buffer);
            }/*else if(0 == i_compareFiles(pf_buffer, pf_original))*/
            else                                                    /*file does exist but has not changed*/
            {
                fclose(pf_buffer);
                fclose(pf_original);
            }/*else*/
            remove("buffer.html");
        }/*if(NULL != pf_buffer)*/
    }/*while(-1 != getline(&pc_url, &s_len, pf_urls))*/

    curl_easy_cleanup(px_curlHandle);
    fclose(pf_urls);

    if( NULL != px_sites)
    {
        GtkApplication *x_app = gtk_application_new("personal.project.DidItChange", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(x_app, "activate", G_CALLBACK(v_createGUI), px_sites);
        g_application_run(G_APPLICATION(x_app), 1, argv);
        g_object_unref(x_app);
        v_freeAll(px_sites);
    }/*if( NULL != px_sites)*/
    return 0;
}/*main*/
