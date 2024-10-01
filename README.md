# C++ Web Server for Nott Course

Find more about the project: [EricWay1024/nott-course: React app of an unofficial enhancement of the course catalogue offered by University of Nottingham](https://github.com/EricWay1024/nott-course).

## Built With

- [CrowCpp/Crow: A Fast and Easy to use microframework for the web.](https://github.com/CrowCpp/Crow)

- [SRombauts/SQLiteCpp: SQLiteC++ (SQLiteCpp) is a smart and easy to use C++ SQLite3 wrapper.](https://github.com/SRombauts/SQLiteCpp)

## Getting Started

All commands tested on Debian Bullseye.

Clone this repo. Note that 
`SQLiteCpp` is included as a submodule of this repo.

```bash
git clone --recurse-submodules https://github.com/EricWay1024/nott-course-server-cpp.git
```

Install `Crow`.

```bash
sudo apt-get install build-essential libboost-all-dev
wget https://github.com/CrowCpp/Crow/releases/download/v1.0%2B3/crow-v1.0+3.deb
sudo apt install ./crow-v1.0+3.deb
rm ./crow-v1.0+3.deb
```

## Building and Running

```bash
mkdir build
cd build
cmake ..
cmake --build .
cp ../data.db .
./nottCourseServer &
```

## Database Specification

`data.db` is a SQLite database with two tables: `course` and `plan`. All fields in both tables are stored as text, but some fields are stringified JSON objects. See table schemas below.

```sql
CREATE TABLE course (
        id BIGINT, 
        code TEXT, 
        title TEXT, 
        credits TEXT, 
        level TEXT, 
        summary TEXT, 
        aims TEXT, 
        offering TEXT, 
        convenor TEXT, 
        semester TEXT, 
        requisites TEXT, 
        "additionalRequirements" TEXT, 
        outcome TEXT, 
        "targetStudents" TEXT, 
        "assessmentPeriod" TEXT, 
        "courseWebLinks" TEXT, 
        class TEXT, 
        assessment TEXT, 
        "belongsTo" TEXT
);
CREATE INDEX ix_course_id ON course (id);

CREATE TABLE IF NOT EXISTS "plan" (
        id BIGINT, 
        title TEXT, 
        "academicPlanCode" TEXT, 
        "ucasCode" TEXT, 
        school TEXT, 
        "planType" TEXT, 
        "academicLoad" TEXT, 
        "deliveryMode" TEXT, 
        "planAccreditation" TEXT, 
        "subjectBenchmark" TEXT, 
        "educationalAimsIntro" TEXT, 
        "educationalAims" TEXT, 
        "outlineDescription" TEXT, 
        "distinguishingFeatures" TEXT, 
        "furtherInformation" TEXT, 
        "planRequirements" TEXT, 
        "includingSubjects" TEXT, 
        "excludingSubjects" TEXT, 
        "otherRequirements" TEXT, 
        "ieltsRequirements" TEXT, 
        "generalInformation" TEXT, 
        modules TEXT, 
        assessment TEXT, 
        "assessmentMarking" TEXT, 
        "progressionInformation" TEXT, 
        "borderlineCriteria" TEXT, 
        "degreeInformation" TEXT, 
        "courseWeightings" TEXT, 
        "degreeCalculationModel" TEXT, 
        "otherRegulations" TEXT, 
        "notwithstandingRegulations" TEXT, 
        overview TEXT, 
        "assessmentMethods" TEXT, 
        "teachingAndLearning" TEXT, 
        "learningOutcomes" TEXT, 
        "degreeType" TEXT, 
        degree TEXT
);
```
The following fields are stringified JSON:

Table `course`:
```json
[
    "convenor",
    "requisites",
    "additionalRequirements",
    "courseWebLinks",
    "class",
    "assessment",
    "belongsTo"
]
```

Table `plan`:

```json
[
    "school",
    "planAccreditation",
    "subjectBenchmark",
    "modules",
    "courseWeightings",
    "degreeCalculationModel"
]
```


