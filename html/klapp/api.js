var teamnames = {};
var teamids = {};
var teams = 0;

function receiveTeams()
{
	$.getJSON("api/teams.json", function(data) {
		var teamcontainer = $('#teamcontainer');
		teamcontainer.html("");
		for (var i = 0; i < data.length; i++)
		{
			teamnames[i] = data[i].name;
			teamids[i] = data[i].nr;
			teams++;
			var html  = '<div class="team" id="team' + data[i].nr + '">';
			html     +=   '<div class="first">';
			html     +=     '<div><span class="rank"><i class="cubes icon"></i>' + data[i].rank + '</span></div>';
			html     +=     '<div><span class="laps">' + data[i].laps + '</span></div>';
			html     +=   '</div>';
			html     +=   '<div class="second">';
			html     +=     '<div>';
			html     +=       '<span class="title">' + data[i].name + '</span>';
			html     +=       '<span class="num titlenum">' + data[i].nr + '</span>';
			html     +=     '</div>';
			for (var j = 0; j < data[i].drivers.length; j++)
			{
				html +=     '<li id="team' + data[i].nr + 'driver' + data[i].drivers[j].nr + '" class="' + (data[i].drivers[j].nr == data[i].currdriver ? 'curr' : '') + '">';
				html +=       '<span class="driver">' + data[i].drivers[j].name + '</span>';
				html +=       '<span class="num drivernum">' + data[i].drivers[j].nr + '</span>';
				html +=     '</li>';
			}
			html     +=   '</div>';
			html     +=   '<div class="third">';
			html     +=     '<div><span class="speed">' + data[i].avg.toFixed(2) + '&nbsp;<sup>km</sup>/<sub>h</sub></span></div>';
			html     +=   '</div>';
			html     += '</div>';
			teamcontainer.append(html);
		}
	});
}

$(document).ready(receiveTeams);


function searchTeams(term)
{
	term = term.toLowerCase();
	console.log('search "' + term + '" in ' + teams + ' teams');
	for (var i = 0; i < teams; i++)
	{
		console.log(teamids[i] + "\t" + teamnames[i]);
		if (teamnames[i].toLowerCase().indexOf(term) > -1)
			$('#team' + teamids[i]).css("display", "");
		else
			$('#team' + teamids[i]).css("display", "none");
	}
}

$(document).ready(function() {
	$("#search").on("input", function() {
		searchTeams($("#search").val());
	});
});
