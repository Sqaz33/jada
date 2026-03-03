with Ada.Text_IO; use Ada.Text_IO;

procedure Bool2 is 
   x: Boolean := true;
   y: Boolean := true;


   function foo(x: Boolean) return Boolean is
   begin
       Put_Line("foo");
       return x;
   end foo;

begin
   Get(x);
   if (x and foo(true)) then
       Put_Line("branch true");
   else
       Put_Line("branch false");
   end if;

   Get(x);
   if (x or foo(false)) then
       Put_Line("branch true");
   else
       Put_Line("branch false");
   end if;

   get(x);
   get(y);
   if ((x and y) or foo(true)) then
       Put_Line("branch true");
   else
       Put_Line("branch false");
   end if;

end Bool2;