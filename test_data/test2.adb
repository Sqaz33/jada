with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type Ty12 is record 
      X: Integer;
   end record;

   function ty12f return Ty12 is
      obj: Ty12;
   begin
      return obj;
   end ty12f;

   objty12: Ty12;

begin
   objty12 := ty12f;
end TestLoops;
