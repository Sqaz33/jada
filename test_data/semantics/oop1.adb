with ada.text_io;

procedure main is 
   package pack is
      type base1 is tagged record 
         W: Integer;
      end record;

      procedure f(q: in out base1);

      type base is new base1 with record 
         X: Integer;
      end record;

      procedure f(q: in out Base);
      
   end pack;

   package body pack is
      procedure f(q: in out Base) is
      begin
         q.X := 1;
      end f;

      procedure f(q: in out Base1) is
      begin
         q.W := 1;
      end f;
   end pack;

   procedure f(q: in out pack.Base1) is -- err если pack.base
   begin
      q.W := 1;
   end f;

   procedure f(q: in out pack.base'class) is
      x: Integer;
   begin
      x := 1;
      --  q.f;
   end f;

   l: pack.base;
   --  l2: pack.base1;

begin
   --  l.f;
   --  f(l);
   --  f(l2);
   f(l);
end main;
