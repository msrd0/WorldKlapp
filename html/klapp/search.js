// load this after api.js and after #search was defined !!!

function searchTeams(term)
{
	term = term.toLowerCase();
	for (var i = 0; i < teams; i++)
	{
		if (teamnames[i].toLowerCase().indexOf(term) > -1)
			$('#team' + teamids[i]).css("display", "");
		else
			$('#team' + teamids[i]).css("display", "none");
	}
}

$("#search").on("input", function() {
	searchTeams($("#search").val());
});
