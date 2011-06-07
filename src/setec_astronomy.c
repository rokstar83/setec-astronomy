/*****************************************************************************/
/* setec_astronomy.c for Setec Astronomy                                     */
/* Copyright (c) 2011 Thomas Hartman (rokstar83@gmail.com)                   */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or the License, or (at your option) any later             */
/* version.                                                                  */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*****************************************************************************/
#include "little_black_box.h"
#include "setec_astronomy.h"
#include "errors.h"
#include "util.h"
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

int add_name_pass(const char * password_file, const char * master_password, 
									const char * name, const char * password)
{
	 struct little_black_box lbb;
	 int err;

	 if( file_exists(password_file) ) {
			err = append_name_pass(password_file, master_password, name, password);
	 } else {
			err = lbb_open(&lbb, SA_CRYPT_MODE, password_file, master_password);
			if( err == SA_SUCCESS ) {
				 err = lbb_write_name_pass(&lbb, name, password);			
			}
			lbb_close(&lbb);
	 }
	 
	 return err;
}

int append_name_pass(const char * password_file, const char * master_password,
										 const char * name, const char * password)
{
	 struct little_black_box r_lbb;
	 struct little_black_box w_lbb;
	 int err;
	 char * temp_password_file;

	 temp_password_file = gen_temp_filename(password_file);
	 if(rename(password_file, temp_password_file) != 0)
			return SA_COULD_NOT_RENAME;

	 err = lbb_open(&r_lbb, SA_DECRYPT_MODE, temp_password_file, master_password);
	 if(err != SA_SUCCESS) {
			rename(temp_password_file, password_file);
			free(temp_password_file);
			return err;
	 }
	 
	 err = lbb_open(&w_lbb, SA_CRYPT_MODE, password_file, master_password);
	 if(err != SA_SUCCESS) {
			rename(temp_password_file, password_file);
			free(temp_password_file);
			lbb_close(&r_lbb);			
			return err;
	 }

	 err = lbb_copy(&r_lbb, &w_lbb);
	 if(err != SA_SUCCESS) {
			rename(temp_password_file, password_file);
			free(temp_password_file);
			lbb_close(&w_lbb);
			lbb_close(&r_lbb);
	 }

	 err = lbb_write_name_pass(&w_lbb, name, password);	 
	 if(err != SA_SUCCESS) 
			rename(temp_password_file, password_file);
	 else
			remove(temp_password_file);
	 
	 free(temp_password_file);
	 lbb_close(&w_lbb);
	 lbb_close(&r_lbb);
	 	 
	 return err;
}

int del_name_pass(const char * password_file, const char * master_password,
									const char * name)
{
	 struct little_black_box r_lbb;
	 struct little_black_box w_lbb;
	 struct name_pass_pair pair;
	 int err;
	 char * temp_password_file;

	 temp_password_file = gen_temp_filename(password_file);
	 if(rename(password_file, temp_password_file) != 0)
			return SA_COULD_NOT_RENAME;

	 err = lbb_open(&r_lbb, SA_DECRYPT_MODE, temp_password_file, master_password);
	 if(err != SA_SUCCESS) {
			rename(temp_password_file, password_file);
			free(temp_password_file);
			return err;
	 }
	 
	 err = lbb_open(&w_lbb, SA_CRYPT_MODE, password_file, master_password);
	 if(err != SA_SUCCESS) {
			rename(temp_password_file, password_file);
			free(temp_password_file);
			lbb_close(&r_lbb);			
			return err;
	 }

	 while((err = lbb_read_pair(&r_lbb, &pair)) == SA_SUCCESS) {
			if(strncmp(pair.name, name, MAX_NAME_LEN) != 0) {
				 err = lbb_write_pair(&w_lbb, pair);
				 if(err != SA_SUCCESS)
						break;
			}
	 }

	 if(err != SA_NO_MORE_PAIRS)
			rename(temp_password_file, password_file);
	 else
			remove(temp_password_file);
	 
	 free(temp_password_file);
	 lbb_close(&w_lbb);
	 lbb_close(&r_lbb);
	 	 
	 return (err > 0 ? SA_SUCCESS : err);
}

int get_pass_by_name(const char * password_file, const char * master_password,
										 const char * name, char * pass)
{
	 struct little_black_box lbb;
	 struct name_pass_pair pair;	 
	 int err;

	 err = lbb_open(&lbb, SA_DECRYPT_MODE, password_file, master_password);
	 if(err != SA_SUCCESS)
			return err;

	 while((err = lbb_read_pair(&lbb, &pair)) == SA_SUCCESS) {
			if(strncmp(pair.name, name, MAX_NAME_LEN) == 0) {
				 strncpy(pass, pair.pass, MAX_PASS_LEN);
				 break;
			}
	 }

	 lbb_close(&lbb);

	 return (err == SA_NO_MORE_PAIRS) ? SA_NAME_NOT_FOUND : err;
}
