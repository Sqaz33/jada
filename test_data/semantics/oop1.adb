with ada.text_io;

procedure main is 
   --  package pack is

   --     procedure f(q: out Base);
   --     procedure f(q: in Base);
   --  end pack;

   package pack is
      type base1 is tagged record 
         W: Integer;
      end record;

      type base is new base1 with record 
         X: Integer;
      end record;

      procedure f(q: in Base) is
      begin
         q.X := 1;
      end f;
   end pack;

   procedure f(q: in pack.Base; q2: pack.base) is
   begin
      q.X := 1;
   end f;

   l: pack.base;

begin

   l.f;
end main;
