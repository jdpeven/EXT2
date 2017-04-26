#include "global.c"
#include "util.c"
#include "type.h"

/*void rpwd(MINODE * mip)
{
   MINODE * parent;
   int i;
   //char myname[128];
   char * myname = malloc(sizeof(char) * 128);
   int parentIno, selfIno, nameLen;
   if((mip->dev == root->dev) && (mip->ino == root->ino)){
           printf("/");
           return;
   }   
   parentIno = nameToIno(mip, "..");
   selfIno = nameToIno(mip, ".");
   parent = iget(mip->dev,parentIno);
   rpwd(parent);
   inoToName(parent, selfIno, &myname);
   nameLen = strlen(myname);
   for(i = 0; i < nameLen; i++){
           putchar(myname);
           myname++;
   }
   printf("%.*s",nameLen, myname);                              //still funky
   printf("/");
   iput(parent);
}*/

void ipwd(MINODE * mip)
{
   MINODE * parent;
   int i;
   char * myname = malloc(sizeof(char) * 128);
   int parentIno, selfIno, nameLen;
   int index = 0;
   char stack[16][100];
   int len;
   //char * stack[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
   //char ** stack = (char*)malloc(16*sizeof(char *));          //assuming path wont have more that 16 elements
   //char myname[128];
   while(1)
   {
        if((mip->dev == root->dev) && (mip->ino == root->ino)){
           strcpy(stack[index], "/");
           //strcpy(stack + index,"/");
           break;
        }
        if(2 == nameToIno(mip, ".") && mip->dev != root->dev)
        {
           ipwd(mip->mptr->mntloc);
           break;
        }
        parentIno = nameToIno(mip, "..");
        //parentIno = getino(&(running->cwd->dev), "..");
        selfIno = nameToIno(mip, ".");
        parent = iget(mip->dev,parentIno);
        //rpwd(parent);
        inoToName(parent, selfIno, &myname);
        strncpy(stack[index], myname, strlen(myname));
        //stack[index][len] = '/0';
        //strncpy(stack+index, myname, strlen(myname));
        iput(parent);
        //iput(parent);                   //no idea if this is what i need to do
        mip = parent;
        index++;
   }
   printf("/");                         //always will have root
   index--;
   for( ; index >= 0; index --)          //index > 0 because stack[0] == '/'
   {
        //len = strlen(stack[index]);
        len = strlen(stack[index]);
        for(i = 0; i < len; i++)
        {
                if((stack[index][i] >= 'a' && stack[index][i] < 'z') || 
                        ((stack[index][i] >= 'A' && stack[index][i] <= 'Z')))
                //if(stack[index][i] != '/0')
                        putchar(stack[index][i]);
                else
                        break;
        }
        putchar('/');
        //printf("%s/", stack[index]);
   }
   iput(parent);
}

int pwd(char * pathname)
{
   printf("Running pwd\n");
   if((running->cwd->dev == root->dev) && (running->cwd->ino == root->ino)){
           printf("/\n");
           return;
   }
   MINODE * temp = running->cwd;
   /*printMinode(running->cwd);
   printMinode(temp);*/
   //////rpwd(temp);
   ipwd(temp);
   printf("\n");
   //iput(temp);
        
   /*Write this function as a recursive fucnction, which
   1. if wd is already the root:
         print /; return;
   2. Get parent's MINODE pointer wd; 
          (HOW? get i_block[0]; then iget(dev, ino of ..))
      Call pwd(wd) again with parent's MINODE pointer;
   3. Print wd's name followed by a /;
          (HOW TO get the name string of a MINODE?)

          2 ways: if you recorded the pathname of a file in its MINODE,
                  then it's the basename() of that string.
          OR    : You have this guy's ino and its parent's MINODE.
                  Search the parent DIR for an entry with this ino. Then, you
                  have its name. I PREFER THIS method !!!

   4. FOR LEVEL-3: If you implement MOUNTing, make sure your recursion can
                     cross mounting points.
   */
}