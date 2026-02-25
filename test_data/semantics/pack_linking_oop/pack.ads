package pack is
   type ty is tagged record 
      X: Integer;
   end record;

   procedure proc(self: in out ty);
end pack;