/**********************************************************************
			     	- NOTICE -
		      AT&T Communications Proprietary	
		   Use Pursuant to Company Instructions
***********************************************************************
**		Name      : new_proc.c
**		Date      : 2/8/89
**		Author    : David Lehmann  C50-51C71
**		Purpose   : create a new process (high level upproc())
**		Calls     : upproc()
**		Changes   :
**********************************************************************/
static char sccs_id[] = "@(#)new_proc.c	3.1	4/14/89	09:26:55";


#include <dmd.h>

Proc *
new_proc(function, new_rect, host, stack_size, argv)
	void (*function)();
	Rectangle new_rect;
	struct Logio *host;
	long stack_size;
	register char *argv[];
{
	Proc *new_p, *upproc(), *locproc();
	Rectangle canon();

	Point char_to_bits();

	/*************************************/
	/***  Fix rectangle, if necessary  ***/
	/*************************************/
	new_rect = canon(new_rect.origin, new_rect.corner);
	rectclip( &new_rect, Rect( 0, 0, XMAX, YMAX) );

	/*******************************************************/
	/***   UNDOCUMENTED FUNCTIONS:  upproc(), locproc()  ***/
	/***   Get new process                               ***/
	/***   upproc()  - With an associated window         ***/
	/***   locproc() - Without associated window         ***/
	/*******************************************************/
	if(!eqrect(new_rect, Rect(0, 0, 0, 0)))
		new_p = upproc( function, new_rect, host, stack_size );
	else
		new_p = locproc(function, stack_size);

	if (new_p)
	{		
		new_p->uDrect = new_p->rect;
		new_p->uJdisplayp = (Bitmap *) new_p->layer;
		new_p->state |= USER;
		new_p->appl = P->appl;
	
		/************************************************************/
		/***  This new process is a part of the calling process   ***/
		/************************************************************/
		new_p->host = host;
		new_p->text = P->text;
		new_p->data = P->data;
		new_p->bss  = P->bss;

		/**********************/
		/***  Pass on argv  ***/
		/**********************/
		if (argv)
		{
			/***************************************************************/
			/***  Duplicate argv and give duplicate to the new process.  ***/
			/***************************************************************/


			int nstrings = 0, space_needed = 0;
			char *alloc();
			void allocown();

			int i;
			register char *space, *data;


			/*************************************************************/
			/***  Calculate amount of space needed to duplicate argv.  ***/
			/*************************************************************/
			for (nstrings = 0; argv[nstrings]; nstrings++)
				space_needed += sizeof(char *) + strlen(argv[nstrings]) + 1;

			/*****************************/
			/***  for NULL terminator  ***/
			/*****************************/
			space_needed += sizeof(char *);

			/*******************************************/
			/***  ONE alloc for argv array and  for  ***/
			/***  the strings the array points to    ***/
			/*******************************************/
			new_p->uargv = (char **)alloc(space_needed);
			new_p->uargc  =  nstrings;

			/*******************************/
			/***  Check if alloc failed  ***/
			/*******************************/
			if (!new_p->uargv)
			{
				/*******************************************/
				/***  UNDOCUMENTED FUNCTION:  _delete()  ***/
				/***  Delete new window.                 ***/
				/*******************************************/
				_delete(new_p);

				return((Proc *)0);
			}

			/*********************************************************/
			/***   Terminate new process' argv array with a NULL   ***/
			/*********************************************************/
			new_p->uargv[nstrings] = (char *)0;

			/*********************************************************************/
			/***  Copy strings from argv to the space created from the alloc.  ***/
			/***  The variable, space, points to the next available byte from  ***/
			/***  the alloc to be used to store the contents to the strings.   ***/
			/*********************************************************************/
			space = (char *) (new_p->uargv + nstrings + 1);

			for (i = 0; i < nstrings; i++)
			{
				new_p->uargv[i] = space;

				data = argv[i];
				while(*data)
					*space++ = *data++;

				*space++ = '\0';
				
			}

			/**************************************************/
			/***  Give the alloc space to the new process.  ***/
			/**************************************************/
			allocown(new_p->uargv, new_p);

			
		}
		else
		{
			/********************************************/
			/***  No argv to pass on to new process.  ***/
			/********************************************/
			new_p->uargv = (char **)0;
			new_p->uargc  =  0;
		}

		/***********************************************************/
		/***  Set default window size for reshape from button 3  ***/
		/***********************************************************/
		if(!eqrect(new_rect, Rect(0, 0, 0, 0)))
			new_p->ctob = char_to_bits;

		/*****************************************************/
		/***  TO BE USED ONLY FOR DEBUGGING                ***/
		/***  To stop PROCESS EXECEPTIONS; use with dmdpi  ***/
		/*****************************************************/
#ifdef DEBUG
		new_p->state |= DONTCRASH;
#endif
	}


	return(new_p);

}



/*****************************************************************/
/***  Use to give default outline on a reshape from button 3.  ***/
/***  See BTOC(3R) in the 630 MTG Software Reference Manual.   ***/
/*****************************************************************/

static Point
char_to_bits(host_x, host_y, p)
	int host_x, host_y;
	Proc *p;
{
	Point pt;

	pt.x = p->layer->rect.corner.x - p->layer->rect.origin.x;
	pt.y = p->layer->rect.corner.y - p->layer->rect.origin.y;

	return(pt);
}
