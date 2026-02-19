with Ada.Text_IO;
use Ada.Text_IO;

procedure Main is
    X: Integer;

   procedure proc(V: Integer) is
      x: Integer;
      Y: Integer := 1;
   begin
      X := Y;
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
   proc(pack.pack.L2.Y.Z);
   X := pack.pack.L2.Y.Z;
end Main;
