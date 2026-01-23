procedure main is
   --  type xtype is record
   --     q: Integer;
   --  end record;

   package pack is 
      type xype is record
         q1: Integer := 1;
      end record;

      package packInner is
         procedure procInner;
      end packInner;


      procedure proc;
   end pack;

   package body pack is 
      procedure proc is
         v: Integer;
      begin
         v := 1;
      end proc;

      package body packInner is
         procedure procInner is 
            q: Integer;
         begin
            q := 1;
         end procInner;
      end packInner;

      t: xype;
      t2: Integer := t.q1;
   end pack;

   --  type x is Integer;

   y: Integer;
begin
   y := 1;
end main;