with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   x: Boolean := false;
   y: Boolean := false;
   

   function foo return Boolean is
      ret: Boolean := false;
   begin
      get(ret);
      return ret;
   end foo;

begin

   Get(x);
   if x then 
      Put_Line("x true");
   else 
      Put_Line("x false");
   end if;

   Get(x);
   Get(y);
   if ((x or y) or foo) then
      Put_Line("x or y or foo - true");
   end if;

   Get(x);
   if (x and foo) then
      Put_Line("x and foo - true");
   end if;
end TestLoops;
