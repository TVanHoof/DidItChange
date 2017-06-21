/*=================================================================
 *project: DidItChange
 *
 *Description: 	Since I am part of many communities
 *							I want to stay up to date with al of their calendars
 *
 *Author: Thomas Van Hoof
 *=================================================================
 *pseudo code for the project:
 *for all urls in url.txt
 *dowload content of html of url into buffer.html
 *compare content content of buffer.html to previously saved file*
 *if different
 *  create clickable popup
 *    if click on popup -> go to site to view the changes
 *    else close after a few seconds
 *  rename buffer.html to other filename to compare to next time
 *else
 *  do nothing
 *=================================================================
*/

#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "curl/curl.h"

static size_t s_write_data(void *pv_data, size_t s_size, size_t s_nmemb, void *pv_stream)
{
	return fwrite(pv_data, s_size, s_nmemb, (FILE*)pv_stream);
}/*s_write_data*/

/*@brief let the string stop at the newline character
 *
 *@param	pc_string	this string will be ended where the \n character is
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
 *@param	fp_file1	FILE pointer to file to compare
 *@param	fp_file2	FILE pointer to file to compare
 *
 *@return true if files are identical, false otherwise
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
		//printf("i_ch1 = %d,\t i_ch_2 = %d\n", i_ch1, i_ch2);
	}while((EOF != i_ch1) && (EOF != i_ch2) && (i_ch1 == i_ch2));

	//fseek(fp_file1, 0, SEEK_SET);
	//do
	//{
	//	i_ch = getc(fp_file1);
	//	printf("%c",i_ch);
	//}while(i_ch != EOF);
	return i_ch1 == i_ch2;
}/*b_compareFiles*/

/*@brief generate a filename for a url
 *
 *@param	pc_filename		the generated filename
 *@param	pc_url				the given url to generate a filename for
 */
static void v_generateFileNameForUrl(char *pc_filename, char *pc_url)
{
	int i_indexFileName = 0;
	int i_indexURL = 4;
	
	/*find the right spot to start copying from*/
	if('s' == pc_url[i_indexURL])	/*check wether the url starts with https or http*/
	{
		i_indexURL = 12;
	}/*if('s' == pc_url[i_indexURL])*/
	else
	{
		i_indexURL = 11;
	}/*else*/

	/*copy everything until the .*/
	for(;pc_url[i_indexURL] != '.'; ++i_indexURL)
	{
		pc_filename[i_indexFileName++] = pc_url[i_indexURL];
	}/*for(;pc_url[i_indexURL] != '.'; ++i_indexURL)*/

	pc_filename[i_indexFileName++] = '_';	/*insert binding*/

	if(strlen(pc_url) - i_indexURL > 5)	/*is there more left then just country code?*/
	{
		/*search for the index of the las '/'*/
		i_indexURL = strlen(pc_url);
		for(;pc_url[i_indexURL] != '/'; --i_indexURL);
		++i_indexURL;
		
		/*copy the rest of the url into the filename and update indexes*/
		memcpy(&pc_filename[i_indexFileName], &pc_url[i_indexURL], strlen(pc_url) - i_indexURL);
		i_indexFileName += strlen(pc_url) - i_indexURL;
		i_indexURL += strlen(pc_url) - i_indexURL;

		/*finish the name with extension ".html" if needed*/
		if(NULL == strstr(pc_filename, ".html"))
		{
			memcpy(&pc_filename[i_indexFileName], ".html\0", strlen(".html\0"));
		}/*if(NULL == strstr(pc_filename, ".html"))*/
	}/*if(strlen(pc_url) - i_indexURL > 5)*/
	else
	{
		memcpy(&pc_filename[i_indexFileName], "index.html\0", strlen("index.html\0"));
	}/*else*/
}/*v_generateFileNameForUrl*/

int main(int argc, char **argv)
{
	if(argc<2)
	{
		fprintf(stderr, "please give a filename as argument\n");
		exit(EXIT_FAILURE);
	}/*if(argc<2)*/
	FILE *pf_urls = fopen(argv[1], "r");
	char *pc_url = NULL;
	size_t s_len;
	int i_counter = 0;
	CURL *px_curlHandle;

	if(NULL == pf_urls)
	{
		fprintf(stderr, "file can not be opened\n");
		exit(EXIT_FAILURE);
	}/*if(NULL == fp_urls)*/

	/*initialize px_curlHandle*/
	curl_global_init(CURL_GLOBAL_ALL);
	px_curlHandle = curl_easy_init();
	
	/*set the needed settings for px_curlHandle*/
	curl_easy_setopt(px_curlHandle, CURLOPT_VERBOSE, 0L); /*display all debug prints*/
	curl_easy_setopt(px_curlHandle, CURLOPT_NOPROGRESS, 1L); /*disable terminal statusbar*/
	curl_easy_setopt(px_curlHandle, CURLOPT_WRITEFUNCTION, s_write_data);	/*set callback*/

	while(-1 != getline(&pc_url, &s_len, pf_urls))
	{
		++i_counter;
		printf("%d : %s", i_counter, pc_url);
		v_breakAtNewline(pc_url);	/*stop string at \n*/
		FILE *pf_buffer = fopen("buffer.html", "w+");
		if(NULL != pf_buffer)
		{
			curl_easy_setopt(px_curlHandle, CURLOPT_WRITEDATA, pf_buffer);	/*set the buffer file to download into*/
			curl_easy_setopt(px_curlHandle, CURLOPT_URL, pc_url);	/*set the url to download*/
			curl_easy_perform(px_curlHandle);
			
			char pc_filename[256] = {0};
			v_generateFileNameForUrl(pc_filename, pc_url);
			printf("%s\n", pc_filename);
			FILE *pf_original = fopen(pc_filename, "r");
			if(NULL == pf_original)																	/*file does not exist yet, so it has just been added*/
			{
				fclose(pf_buffer);
				rename("buffer.html", pc_filename);
			}/*if(NULL == pf_original)*/
			else if(0 == i_compareFiles(pf_buffer,pf_original))		/*file does exist and has changed*/
			{
				fclose(pf_original);
				fclose(pf_buffer);
				rename("buffer.html", pc_filename);
			}/*else if(0 != i_compareFiles(pf_buffer, pf_original))*/
			else																										/*file does exist but has not changed*/
			{
				fclose(pf_buffer);
				fclose(pf_original);
			}/*else*/
			remove("buffer.html");
		}/*if(NULL != pf_buffer)*/
	}/*while(-1 != getline(&pc_url, &s_len, pf_urls))*/

	curl_easy_cleanup(px_curlHandle);
	fclose(pf_urls);
	return 0;
}/*main*/
