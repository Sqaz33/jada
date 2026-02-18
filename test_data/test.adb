with Ada.Text_IO;
use Ada.Text_IO;

procedure Main is
   Y: Integer := 1;
   X: Integer := Y;

   procedure proc(V: Integer) is
   begin
      X := V;
   end proc;

begin
   proc(1);
end Main;
