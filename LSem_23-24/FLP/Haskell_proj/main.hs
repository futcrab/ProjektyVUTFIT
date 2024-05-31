-- flp-fun
-- xduric05
-- Peter Durica
-- 2024

-- Usage for imported functions:
-- getArgs to get arguments from command line
import System.Environment (getArgs)
-- exitFailure to end program on error
import System.Exit (exitFailure)
-- isPrefixOf for checking spaces before Node or Leaf when converting text to decision tree
-- sort for sorting lists of numbers
-- nub for filtering lists for unique values
-- elemIndex for finding index of element in list
import Data.List (isPrefixOf, sort, nub, elemIndex)
-- splitOn for splitting strings to have separate numbers
import Data.List.Split (splitOn)

-- Datatype for Decision Tree
-- LF ClassName
-- ND featureIndex threshold Tree1 Tree2
data DecisionTree = ET
                |   LF String
                |   ND Int Double DecisionTree DecisionTree
                deriving(Show)
-- Datatypes for Values (One used in classification, other in training)
-- CV featureValues
-- TV featureValues className
data ClassValue = CV [Double] deriving(Show)
data TrainValue = TV [Double] String deriving(Show)

-- function that prints error message
errorMessage :: IO()
errorMessage = do
    putStrLn "Incorrect usage of the program"
    putStrLn "Correct usage: ./flp-fun -1 <file with tree> <file with new data>"
    putStrLn "Or:            ./flp-fun -2 <file with training data>"

-- Functions used in first mode
------------------------------------------------------------------------------------------------------------
findNext :: [String] -> String -> Int -> [String]
--Starts on first line
findNext (x:xs) wChars 1 =
     --if starts on right number of spaces and node or leaf return it
    if isPrefixOf (wChars ++ "Node") x || isPrefixOf (wChars ++ "Leaf") x 
    then x:xs
    --else continue to next line
    else findNext xs wChars 1
--Starts from second line
findNext (_:y:xs) wChars 2 =
    --if starts on right number of spaces and node or leaf return it
    if isPrefixOf (wChars ++ "Node") y || isPrefixOf (wChars ++ "Leaf") y 
    then y:xs
    --else continue to next line
    else findNext xs wChars 1
findNext _ _ _ = []

getValuesFromNode :: String -> (Int, Double)
getValuesFromNode x = do
    -- Split line into separate numbers and then convert them to right types
    let numbers = splitOn [','] x
    (read (numbers !! 0) :: Int, read (numbers !! 1) :: Double)

-- Return string without spaces
removeSpaces :: String -> String
removeSpaces xs = [x | x <- xs, not (elem x " ")]

constructTree :: [String] -> String -> DecisionTree
constructTree (x:xs) wChars = do
    --remove spaces from line for easier parsing
    let line = removeSpaces x
    -- if starts on node
    if isPrefixOf "Node:" line
        then do
            -- get feature index and threshold from line, droping first 5 chars because they are "Node:"
            let (featureIndex, featureThreshold) = getValuesFromNode (drop 5 line)
            -- create node with gathered values and recursively build the tree
            -- first branch always start on next line, second branch needs to be found, passing on spaces before node + 2 new spaces to identify branch
            ND featureIndex featureThreshold (constructTree xs ("  " ++ wChars)) (constructTree (findNext xs ("  " ++ wChars) 2) ("  " ++ wChars))
        -- if starts on leaf create leaf with className, droping first 5 chars because they are "Leaf:"
        else LF (drop 5 line)
-- If empty create simple leaf ()
constructTree [] _ = LF "A"

constructClassData :: [String] -> [ClassValue]
constructClassData (x:xs) = do
    -- Split line on char "," right after removing unnecessary spaces
    let line = splitOn [','] (removeSpaces x)
    -- construct a list of doubles from list of strings
    let lineValues = map read line :: [Double]
    -- create new value of type ClassValue with constructed list and bind it with the rest
    (CV lineValues):constructClassData xs
-- At the end return empty list
constructClassData [] = []

classValuesToTree :: [ClassValue] -> DecisionTree -> DecisionTree -> IO ()
-- if node check value on index compared to threshold
classValuesToTree (CV values:xs) (ND featureIndex featureThreshold t1 t2) tStart = 
    if values !! featureIndex >= featureThreshold
    -- based on if select branch, keep highest node for next classValues
    then classValuesToTree (CV values:xs) t2 tStart
    else classValuesToTree (CV values:xs) t1 tStart
-- if leaf print className and start again for next classValue
classValuesToTree (_:xs) (LF className) tStart = 
    putStrLn className >> classValuesToTree xs tStart tStart
-- At end return
classValuesToTree _ _ _ = return ()

-- Classification of values based on tree
firstMode :: [String] -> IO()
firstMode [tree, classData] = do
    --Read file with decision tree and separate it to lines
    treeWhole <- readFile tree
    let treeLines = lines treeWhole

    --Read file with values and separate it to lines
    classWhole <- readFile classData
    let classLines = lines classWhole

    -- Construct tree and class data based on input files
    let classValues = constructClassData classLines
    let decTree = constructTree treeLines ""
    
    -- Class obtained data based on decision tree
    classValuesToTree classValues decTree decTree
firstMode _ = errorMessage >> exitFailure

-- Functions used in second mode
------------------------------------------------------------------------------------------------------------
constructTrainData :: [String] -> [TrainValue]
constructTrainData (x:xs) = do
    -- remove unnecessary spaces from line and then split to separate numbers
    let line = splitOn [','] (removeSpaces x)
    -- save className which is always last
    let className = last line
    -- save featureValues without className
    let lineValues = map read (init line) :: [Double]
    -- create data of type TrainValue and recursively call func to rest of the lines
    (TV lineValues className):constructTrainData xs
-- on end of lines return empty list
constructTrainData [] = []

checkSameClass :: [TrainValue] -> Bool
-- If only one or zero values return True
checkSameClass [TV _ _] = True
checkSameClass [] = True
-- If not check all strings and return value
checkSameClass (TV _ className_1: TV _ className_2: xs) = 
    className_1 == className_2 && checkSameClass (TV [] className_2: xs)

-- get values from all training data at specific index and combine them to list
getValuesByFeatureIndex :: [TrainValue] -> Int -> [Double]
getValuesByFeatureIndex (TV x _:xs) featureIndex =
    (x !! featureIndex):getValuesByFeatureIndex xs featureIndex
getValuesByFeatureIndex [] _ = []

-- returns list of lists of doubles, list of doubles on index 0 is all features of data on index 0, list of doubles on index 1 is all features of data on index 1 and so on...
getFeatureLists :: [TrainValue] -> Int -> [[Double]]
getFeatureLists (TV x _:xs) featureIndex =
    -- check if we are trying get index inside of the list
    if length x > featureIndex
    -- if yes get all the values on that index and then recursively call on next index
    then getValuesByFeatureIndex (TV x "":xs) featureIndex:getFeatureLists (TV x "":xs) (featureIndex + 1)
    -- if not return empty list
    else []
getFeatureLists [] _ = []

-- connect all classNames to single list
getClassNames :: [TrainValue] -> [String]
getClassNames (TV _ className:xs) = className:getClassNames xs
getClassNames [] = []

-- Calculate thresholds by averaging two numbers next to each other in sorted list
getThresholds :: [Double] -> [Double]
getThresholds (x:y:xs) = ((x+y)/2) : getThresholds (y:xs)
getThresholds _ = []

-- my len function which calculates length of list in double
myLen :: [a] -> Double
myLen (_:xs) = 1.0 + myLen xs
myLen [] = 0.0

-- calculate probability for single action
-- p = (len p_a/ len x)^2
calculateProbability :: [String] -> String -> Double
calculateProbability x y = do
    let classList = [className | className <- x, className == y]
    (myLen classList/myLen x) * (myLen classList/myLen x)

-- calculate gini index for one branch
-- gini index_sb = 1 - p_a^2 - p_b^2 - p_c^2 ...
calculateGini :: [String] -> [String] -> Double
calculateGini x y = 1.0 - sum[calculateProbability x className | className <- y]

getGiniIndexForThreshold :: [(Double, String)] -> [String] -> [Double] -> [Double]
getGiniIndexForThreshold x uniqueClassNames (threshold:rest) = do
    -- create two lists... one is values smaller than threshold other list is values bigger than threshold
    let smaller = [className | (value, className) <- x, value < threshold]
    let bigger = [className  | (value, className) <- x,value >= threshold]
    -- calculate size of all values
    let size = myLen x
    -- calculate weighted gini index for both branches
    -- gini index_bb = Gini_s * (len s/len x) + Gini_b * (len b/len x)
    let giniIndex = (calculateGini smaller uniqueClassNames) * (myLen smaller / size) + (calculateGini bigger uniqueClassNames) * (myLen bigger / size)
    -- add to list and recursively call func for rest of thresholds
    giniIndex:getGiniIndexForThreshold x uniqueClassNames rest
getGiniIndexForThreshold _ _ [] = []

-- convert Just Int to Int value
getInt :: Maybe Int -> Int
getInt (Just x) = x
getInt _ = 0

-- get gini indexes for all features
getGiniIndexes :: [[Double]] -> [String] -> [(Double, Double)]
getGiniIndexes (x:xs) classNames = do
    -- calculate thresholds with sorted feature values
    let thresholds = getThresholds (sort x)
    -- get gini indexes for different thresholds
    let giniIndexes = getGiniIndexForThreshold (zip x classNames) (nub classNames) thresholds
    -- get smallest gini index (Method explained in function bellow)
    let smallestGiniIndex = getInt (elemIndex (head (sort giniIndexes)) giniIndexes)
    -- create tuple of best gini index and threshold for this feature and recursively call funtion for next feature
    ((giniIndexes !! smallestGiniIndex),(thresholds !! smallestGiniIndex)) : getGiniIndexes xs classNames
getGiniIndexes [] _ = []

generateNode :: [TrainValue] -> DecisionTree
generateNode x = do
    -- get List of lists of doubles... each list of doubles represent values of feature of all training data on given index
    let featureLists = getFeatureLists x 0
    -- get all classNames of all training data
    let classNames = getClassNames x
    -- calculate best gini indexes for all features and save them as a list of tuples as [(gini index, threshold for that index)]
    let giniIndexes = getGiniIndexes featureLists classNames
    -- get index of a smallest value in a list
    -- method:
    -- 1. sort list of gini indexes from min values to max
    -- 2. get first value of sorted list which is smallest
    -- 3. use elemIndex to get index of a smallest value
    -- 4. elem index return Maybe datatype so convert to Int
    -- I've had trouble making a function which returns index of smallest value and i know this is not optimal but it works
    let smallestIndex = getInt (elemIndex (head (sort [gini | (gini, _) <- giniIndexes])) [gini | (gini, _) <- giniIndexes])
    -- get threshold of smallest gini index
    let threshold = snd (giniIndexes !! smallestIndex)
    -- create node with calculated index and threshold and recursively call generateDecTree func with only trainValues which are smaller/bigger than threshold
    ND smallestIndex threshold (generateDecTree [(TV values string) | (TV values string) <- x, (values !! smallestIndex) < threshold])
                               (generateDecTree [(TV values string) | (TV values string) <- x, (values !! smallestIndex)>= threshold])

generateDecTree :: [TrainValue] -> DecisionTree
generateDecTree (TV trainValues className:xs) =
    -- check if all training values are same class
    if checkSameClass (TV trainValues className:xs)
    -- if yes create a leaf if not generate a node
    then LF className
    else generateNode (TV trainValues className:xs)
generateDecTree [] = ET

--print rest of the tree
printLineOfTree :: DecisionTree -> String -> String
-- next lines need to be indexed by spaces and need to have newline char at beginning
-- otherwise same as first lines
printLineOfTree (ND featureIndex featureThreshold smaller bigger) prefix = 
    "\n" ++ prefix ++ "Node: " ++ show featureIndex ++ ", " ++ show featureThreshold ++ 
        (printLineOfTree smaller ("  " ++ prefix)) ++ (printLineOfTree bigger ("  " ++ prefix))
printLineOfTree (LF className) prefix = "\n" ++ prefix ++ "Leaf: " ++ className 
printLineOfTree _ _ = "" 

-- print first line of decision tree
printDecisionTree :: DecisionTree -> IO ()
-- if node print first line with feature index and threshold and then call functions to print branches
printDecisionTree (ND featureIndex featureThreshold smaller bigger) =
    putStrLn $ "Node: " ++ show featureIndex ++ ", " ++ show featureThreshold ++ 
        (printLineOfTree smaller "  ") ++ (printLineOfTree bigger "  ")
-- if leaf print first line with className and end
printDecisionTree (LF className) = putStrLn $ "Leaf: " ++ className
printDecisionTree _ = return()

secondMode :: [String] -> IO()
secondMode [trainData] = do
    -- Read file with training data and separate individual lines
    trainWhole <- readFile trainData
    let trainLines = lines trainWhole

    -- construct trainValues from file
    let trainValues = constructTrainData trainLines

    -- generate decision tree and then print it
    printDecisionTree(generateDecTree trainValues)
secondMode _ = errorMessage >> exitFailure
------------------------------------------------------------------------------------------------------------
-- Main function of the program
main :: IO()
main = do
    -- get arguments from comand line
    args <- getArgs
    -- if no arguments print error message and end
    if null args
        then errorMessage >> exitFailure
        else return ()
    -- check first argument for mode if incorrect then print error message and end
    let (mode:files) = args
    case mode of
        -- based on mode run functions with rest of the arguments passed on
        "-1" -> firstMode files
        "-2" -> secondMode files
        _    -> errorMessage >> exitFailure