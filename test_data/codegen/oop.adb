with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is

   package pack1 is
      type cls1 is tagged record 
         x: Integer := 1;
      end record;
      
      procedure foo(x: in out cls1);

      type cls2 is new cls1 with record 
         y: Integer := 1;
      end record;

      procedure foo(x: in out cls2);

   end pack1;

   package body pack1 is
      procedure foo(x: in out cls1) is
      begin
         Put_Line(Integer'Image(x.x));
         --  x.x := 5;
         --  Put_Line(Integer'Image(x.x));
      end foo;

      procedure foo(x: in out cls2) is
      begin
         Put_Line("Hello World!");
      end foo;
   end pack1;

   procedure proc(obj: in out pack1.cls1'Class) is
   begin
      obj.foo;
   end proc;

   obj1: pack1.cls1;
   obj2: pack1.cls2;
begin

   -- массив надкласоовый
   -- parent call
   --  obj1.foo;

   proc(obj1);
   proc(obj2);

end TestLoops;
