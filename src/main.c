/*****************************************************************************/
/* main.c for Setec Astronomy                                                */
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
#include <stdio.h>
#include <stdlib.h>
#include <mcrypt.h>
#include <mutils/mhash_config.h>
#include <mutils/mhash.h>
#include <string.h>
#include "setecAstronomy.h"
#include "header.h"
#include "key.h"

int main(int argc, char ** argv)
{
	 struct name_pass_pair pair;

	 init_name_pass_pair(&pair);
	 strncpy(pair.name, "TestName\0", MAX_NAME_LEN);
	 strncpy(pair.pass, "TestPassword\0", MAX_PASS_LEN);
	 char password[] = "foobarbaz\0";
	 return add_name_pass_pair(&pair, "/home/thartman/setecAstronomy",password);

	 return 0;
}
