// $("#searchButton").on("click", function() {
$("#search").on("input", function() {
    for (var i = 0; i < $(".team").size(); i++)
    {
        var teamid = $(".team")[i].id;
        var teamname = $("#" + teamid + " td.name").text();
        console.log(teamname);
        if (teamname.toLowerCase().indexOf($("#search").val().toLowerCase()) > -1)
        {
            $("#" + teamid).css("display", "table-row");
            $("." + teamid).css("display", "table-row");
        }
        else
        {
            $("#" + teamid).css("display", "none");
            $("." + teamid).css("display", "none");
        }
    }
});
