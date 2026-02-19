with Ada.Text_IO;
use Ada.Text_IO;

procedure Main is
   X: Integer;

   arr: array(1..3) of Integer;

   procedure proc(V: Integer) is
      x: Integer;
      Y: Integer := 1;
   begin
      X := Y;
   end proc;

   function proc(V: Integer) return Integer is -- no err with this
   begin
      return 1;
   end proc;

   type TyN is record
      Z: Integer;
   end record;

   type Ty is record
      Y: TyN;
   end record;

   package pack is
      package pack is -- err here
         L2: Ty;
      end pack;
   end pack;

begin
   -- X := Integer'Image(1);
   -- 1;
   X := proc(1);
   proc(proc(1)); -- err
   -- arr(1) := 1;
   -- X := pack.pack.L2.Y.Z;
end Main;
