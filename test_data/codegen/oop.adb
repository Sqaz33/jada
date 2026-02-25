with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is

   package pack1 is
      type cls1 is tagged record 
         x: Integer := 1;
      end record;
      
      procedure foo(x: in out cls1);

   end pack1;

   package body pack1 is
      procedure foo(x: in out cls1) is
      begin
         Put_Line(Integer'Image(x.x));
         x.x := 5;
         Put_Line(Integer'Image(x.x));
      end foo;
   end pack1;


   obj1: pack1.cls1;
begin
   obj1.foo;

end TestLoops;
