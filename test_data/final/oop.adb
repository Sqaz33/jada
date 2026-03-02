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

      type cls3 is new cls1 with record 
         y: Integer := 1;
      end record;

      procedure foo(x: in out cls3);

   end pack1;

   package body pack1 is
      procedure foo(x: in out cls1) is
      begin
         Put_Line("cls1::foo");
      end foo;

      procedure foo(x: in out cls2) is
      begin
         Put_Line("cls2::foo");
      end foo;

      procedure foo(x: in out cls3) is
      begin
         Put_Line("cls3::foo");
      end foo;
   end pack1;

   type refarr is array(1..3) of pack1.cls1'Class;

   procedure proc(arr: in out refarr) is
   begin
      arr(1).foo;
      arr(2).foo;
      arr(3).foo;
   end proc;

   obj1: pack1.cls1;
   obj2: pack1.cls2;
   obj3: pack1.cls3;

   ra: refarr;
begin

   ra(1) := obj1;
   ra(2) := obj2;
   ra(3) := obj3;
   proc(ra);  

end TestLoops;


