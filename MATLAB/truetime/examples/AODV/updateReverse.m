function nextHop = updateReverse(RREP, tableID)

global routing_table

table = routing_table{tableID};

entry = [];

l = length(table);

for k = 1:l
  
  if (table{k}.dest==RREP.src)
    table{k}.exptime = ttCurrentTime + RREP.lifetime;
    
    ind = find(table{k}.nbor==RREP.intermed);
    if (isempty(ind))
      table{k}.nbor = [table{k}.nbor RREP.intermed];
    end
    nextHop = table{k}.nextHop;

    routing_table{tableID} = table;
    updateExpiryTimer(tableID);    
    break;
  end
end




