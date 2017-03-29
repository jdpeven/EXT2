

int pwd(char * pathname)
{

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