document.addEventListener("DOMContentLoaded", () => {
    const form = document.getElementById("travelForm");
    const resultSection = document.getElementById("resultSection");

    if (!form) {
        return;
    }

    form.addEventListener("submit", async (event) => {
        event.preventDefault();

        const startLocation = document.getElementById("startLocation").value.trim();
        const finalLocation = document.getElementById("finalLocation").value.trim();
        const availableTime = parseFloat(
            document.getElementById("availableTime").value
        );
        const travelSpeed = parseFloat(
            document.getElementById("travelSpeed").value
        );

        if (!startLocation || !finalLocation) {
            alert("Please enter the starting and final locations.");
            return;
        }

        if (
            isNaN(availableTime) ||
            availableTime <= 0 ||
            isNaN(travelSpeed) ||
            travelSpeed <= 0
        ) {
            alert("Please enter valid time and travel speed.");
            return;
        }

        const inputData = {
            startLocation,
            finalLocation,
            availableTime,
            travelSpeed
        };

        try {
            const response = await fetch("/api/branch-and-bound", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify(inputData)
            });

            if (!response.ok) {
                throw new Error("Unable to calculate the travel plan.");
            }

            const result = await response.json();

            displayResult(result);

        } catch (error) {
            console.error(error);
            alert("Could not connect to the travel planner.");
        }
    });

    function displayResult(result) {
        if (!resultSection) {
            return;
        }

        resultSection.style.display = "block";

        const routeElement = document.getElementById("route");
        const distanceElement = document.getElementById("totalDistance");
        const travelTimeElement = document.getElementById("travelTime");
        const explorationTimeElement =
            document.getElementById("explorationTime");
        const totalTimeElement = document.getElementById("totalTime");
        const valueElement = document.getElementById("totalValue");

        if (routeElement) {
            if (Array.isArray(result.route)) {
                routeElement.textContent = result.route.join(" → ");
            } else {
                routeElement.textContent = result.route || "No route found";
            }
        }

        if (distanceElement) {
            distanceElement.textContent =
                `${result.totalDistance ?? 0} km`;
        }

        if (travelTimeElement) {
            travelTimeElement.textContent =
                formatTime(result.travelTime ?? 0);
        }

        if (explorationTimeElement) {
            explorationTimeElement.textContent =
                formatTime(result.explorationTime ?? 0);
        }

        if (totalTimeElement) {
            totalTimeElement.textContent =
                formatTime(result.totalTime ?? 0);
        }

        if (valueElement) {
            valueElement.textContent =
                result.totalValue ?? 0;
        }

        displayVisualization(result);
    }

    function formatTime(hours) {
        const totalMinutes = Math.round(hours * 60);

        const hoursPart = Math.floor(totalMinutes / 60);
        const minutesPart = totalMinutes % 60;

        if (hoursPart === 0) {
            return `${minutesPart} min`;
        }

        if (minutesPart === 0) {
            return `${hoursPart} hr`;
        }

        return `${hoursPart} hr ${minutesPart} min`;
    }

    function displayVisualization(result) {
        if (
            typeof window.showBranchAndBoundVisualization === "function"
        ) {
            window.showBranchAndBoundVisualization(
                result.searchTree || []
            );
        }
    }
});
