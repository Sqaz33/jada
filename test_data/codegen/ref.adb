with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr is array(1.2) of Integer; 

   procedure foo(x: in out Integer) is
      x2: Integer;
   begin
      x := 2;
   end foo;

   procedure foo(x: in out arr) is
   begin
      x(1) := 1;
   end foo;

   procedure foo(x: String) is
   begin 
      x(1) := 'c'
   end foo;

   type rec tagged record is
      x: Integer := 1;
   end record;

   l: Integer := 1114;
   a: arr := (1, 2);
   s: String;;
begin
   Put_Line(Integer'Image(v));
   foo(v);
   Put_Line(Integer'Image(v));


   Put_Line("==========");
   Put_Line(Integer'Image(l));
   foo3(l);
   Put_Line(Integer'Image(l));
   Put_Line("==========");

end TestLoops;


-- флоат: все операции 

-- многомерные массивы: агрегатная и заливка по контору



