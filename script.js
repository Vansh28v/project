const display = document.getElementById("display");
const historyList = document.getElementById("historyList");

let history = [];

function appendValue(value) {
    display.value += value;
}


function clearDisplay() {
    display.value = "";
}

function deleteLast() {
    display.value = display.value.slice(0, -1);
}


function calculate() {

    if (display.value === "") return;

    try {

        let expression = display.value;

        let result = eval(
            expression
                .replace(/÷/g, "/")
                .replace(/×/g, "*")
        );

        history.unshift(expression + " = " + result);

        if (history.length > 10) {
            history.pop();
        }

        updateHistory();

        display.value = result;

    }

    catch {

        display.value = "Error";

    }

}


function updateHistory() {

    historyList.innerHTML = "";

    history.forEach(item => {

        const li = document.createElement("li");

        li.textContent = item;

        li.onclick = function () {

            const parts = item.split(" = ");

            display.value = parts[1];

        };

        historyList.appendChild(li);

    });

}

const delBtn = document.getElementById("delBtn");

delBtn.addEventListener("dblclick", function () {

    history = [];
    updateHistory();

});

let pressTimer;

delBtn.addEventListener("mousedown", function () {

    pressTimer = setTimeout(function () {

        history = [];

        updateHistory();

    }, 1000);

});

delBtn.addEventListener("mouseup", function () {
    clearTimeout(pressTimer);
});

delBtn.addEventListener("mouseleave", function () {
    clearTimeout(pressTimer);
});