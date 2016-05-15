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
				            + '" class="curr' + ' '
							+ ((data[i].drivers[j].nr == data[i].currdriver) ?  (data[i].drivers[j].last < data[i].avg ? "slow" : "quick") : "") + '">';
				html +=       '<span class="driver">' + data[i].drivers[j].name + '</span>';
				html +=       '<span class="driverlaps">' + data[i].drivers[j].laps + '</span>';
				html +=   '<div class="spacer"></div>';
				if (data[i].drivers[j].nr == data[i].currdriver)
				{
					html +=   '<div class="speedcontainer"><span class="driverspeed">' + data[i].drivers[j].last.toFixed(2) + '&nbsp;<sup>km</sup>/<sub>h</sub></span></div>';
				}
				else
				{
					html +=   '<div class="speedcontainer"><span class="driverspeed">&Oslash;&nbsp;' + data[i].drivers[j].avg.toFixed(2) + '&nbsp;<sup>km</sup>/<sub>h</sub></span></div>';
				}
				html +=     '</li>';
			}
			html     +=   '</div>';
			html     += '</div>';
			teamcontainer.append(html);
		}
		
		var statuscontainer = $('#statuscontainer');
		statuscontainer.html("");
		var mname, mlaps = 0;
		var qname, qavg = 0;
		var aname, aavg = 0;
		var alllaps = 0;
		for (var i = 0; i < data.length; i++)
		{
			alllaps += data[i].laps;
			for (var j = 0; j < data[i].drivers.length; j++)
			{
				if (data[i].drivers[j].laps > mlaps)
				{
					mlaps = data[i].drivers[j].laps;
					mname = data[i].drivers[j].name;
				}
				if (data[i].drivers[j].best > qavg)
				{
					qavg = data[i].drivers[j].best;
					qname = data[i].drivers[j].name;
				}
				if (data[i].drivers[j].avg > aavg)
				{
					aavg = data[i].drivers[j].avg;
					aname = data[i].drivers[j].name;
				}
			}
		}
		statuscontainer.append("<div><b>Meiste Runden:</b> <i>" + mlaps + "</i> von <i>" + mname + "</i></div>");
		statuscontainer.append("<div><b>Schnellste Runde:</b> <i>" + qavg.toFixed(2) + " <sup>km</sup>/<sub>h</sub></i> von <i>" + qname + "</i></div>");
		statuscontainer.append("<div><b>Höchster Durchschnitt:</b> <i>" + aavg.toFixed(2) + " <sup>km</sup>/<sub>h</sub></i> von <i>" + aname + "</i></div>");
		statuscontainer.append("<div><b>Insgesamt:</b> <i>" + alllaps + "</i> Runden (<i>" + (0.45 * alllaps).toFixed(2) + " km</i>)</div>");
	});
}

$(document).ready(receiveTeams);
