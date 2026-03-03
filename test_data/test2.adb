with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   procedure f(x: in out Integer) is
   begin
      f(x);
   end f;

begin
   f(1);
end TestLoops;
