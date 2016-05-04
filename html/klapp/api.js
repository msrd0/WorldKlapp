function receiveTeams()
{
	$.getJSON("api/teams.json", function(data) {
		var teamcontainer = $('#teamcontainer');
		teamcontainer.html("");
		for (var i = 0; i < data.length; i++)
		{
			var html  = '<div class="team">';
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
				html +=     '<li>';
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
