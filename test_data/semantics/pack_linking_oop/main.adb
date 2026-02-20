with pack;

procedure main is 
   obj: pack.ty;

   procedure proc2(x: in out pack.ty'Class) is
   begin
      x.proc;
   end proc2;

   package pack2 is
      type Ty2 is new pack.ty with record
         Y: Integer;
      end record;
   end pack2;

   obj2: pack2.Ty2;
   
begin
   obj.proc;
   proc2(obj);
   proc2(obj2);
   obj2.Y := 1;
end main;