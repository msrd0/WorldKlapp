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
			html     +=     '<div><span class="rank"><i class="cubes icon"></i>&nbsp;' + data[i].rank + '</span></div>';
			html     +=     '<div><span class="laps">' + data[i].laps + '</span></div>';
			html     +=   '</div>';
			html     +=   '<div class="second">';
			html     +=     '<div class="titleline">';
			html     +=       '<li>';
			html     +=         '<span class="title">' + data[i].name + '</span>';
			html     +=         '<span class="num titlenum">' + data[i].nr + '</span>';
			html     +=       '</li>';
			html     +=       '<div><span class="speed">&Oslash;&nbsp;' + data[i].avg.toFixed(2) + '&nbsp;<sup>km</sup>/<sub>h</sub></span></div>';
			html     +=     '</div>';
			for (var j = 0; j < data[i].drivers.length; j++)
			{
				if (data[i].drivers[j].name == "dummy")
					continue;
				html +=     '<li id="team' + data[i].nr + 'driver' + data[i].drivers[j].nr
				            + '" class="' + (data[i].drivers[j].nr == data[i].currdriver ? 'curr' : '')
				            + ' ' + (data[i].drivers[j].avg < data[i].avg ? "slow" : "quick") + '">';
				html +=       '<span class="driver">' + data[i].drivers[j].name + '</span>';
				html +=       '<span class="driverlaps">' + data[i].drivers[j].laps + '</span>';
				if (data[i].drivers[j].nr == data[i].currdriver)
				{
					html +=   '<div class="spacer"></div>';
					html +=   '<div class="speedcontainer"><span class="driverspeed">&Oslash;&nbsp;' + data[i].drivers[j].avg.toFixed(2) + '&nbsp;<sup>km</sup>/<sub>h</sub></span></div>';
				}
				html +=     '</li>';
			}
			html     +=   '</div>';
			html     += '</div>';
			teamcontainer.append(html);
		}
	});
}

$(document).ready(receiveTeams);
