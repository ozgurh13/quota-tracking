
{-# LANGUAGE  ViewPatterns         #-}
{-# LANGUAGE  BlockArguments       #-}
{-# LANGUAGE  ImportQualifiedPost  #-}

import Data.Functor (($>))
import Data.Functor.Identity (Identity)

import Control.Concurrent (threadDelay)
import Control.Monad (void, forever)

import Text.Parsec
import Text.Parsec.Token    qualified as Token
import Text.Parsec.Language

import System.IO
import System.Exit (exitSuccess)
import System.Process (system)


fifoFile :: String
fifoFile = "/tmp/quota.fifo"

writeFifo :: String -> IO ()
writeFifo = writeFile fifoFile


main :: IO ()
main = shell


shell :: IO ()
shell = forever do
    prompt >>= executeCommand


executeCommand :: String -> IO ()
executeCommand (parseInput -> command) = case command of
    ListProcess -> writeFifo "list"

    ListInactiveProcess -> writeFifo "list inactive"

    AddDevice deviceName mode -> writeFifo $ unwords ["add", deviceName, show mode]

    DeleteId pid -> writeFifo $ unwords ["delete", "id", show pid]

    DeleteDevice device -> writeFifo (unwords ["delete", "device", device])

    DeleteAll -> writeFifo "delete all"

    DeleteInactive -> writeFifo "delete inactive"

    LoadFile filePath -> executeAllCommands =<< lines <$> readFile filePath

    Exit -> exitSuccess

    Help -> showHelp

    ClearScreen -> void $ system "clear"

    Terminate -> writeFifo "terminate"

    EnterKey -> pure ()

    Invalid invalid -> putStrLn $ "invalid command: " ++ invalid



prompt :: IO String
prompt = putStr "~> " >> hFlush stdout >> getLine



executeAllCommands :: [String] -> IO ()
executeAllCommands       []   = pure ()
executeAllCommands (cmd:cmds) = do
    executeCommand cmd
    threadDelay 10000
    executeAllCommands cmds




showHelp :: IO ()
showHelp = putStrLn $ unlines
    [ "shell commands"
    , ""
    , " ~> list"
    , "  list all running processes"
    , ""
    , " ~> list inactive"
    , "  list inactive processes"
    , ""
    , " ~> add 'device' 'mode'"
    , "  start listening to 'device' in mode 'mode'"
    , "  devices: the interface name"
    , "  mode: either 2 (layer 2: mac) or 3 (layer 3: ip)"
    , "  example: add lo 2"
    , ""
    , " ~> delete id 'n'"
    , "  stop listening to process with id 'n'"
    , "  example: delete id 13"
    , ""
    , " ~> delete device 'device'"
    , "  stop listening to device 'device'"
    , "  example: delete device wlp2s0"
    , ""
    , " ~> delete all"
    , "  delete all running processes"
    , ""
    , " ~> delete inactive"
    , "  delete all inactive processes"
    , ""
    , " ~> load 'filename'"
    , "  load commands from file"
    , "  example: load \"myfile\""
    , ""
    , " ~> clear"
    , "  clear the shell screen"
    , ""
    , " ~> exit"
    , " ~> quit"
    , "  exit the shell"
    , ""
    , " ~> terminate"
    , "  terminate the program"
    , ""
    , " ~> help"
    , "  show this (help) message"
    ]




-- Parser --

type Parser = Parsec String ()

type DeviceName = String
data DeviceMode = Mode2 | Mode3

instance Show DeviceMode where
    show Mode2 = "2"
    show Mode3 = "3"



type Device = String

data Command
    = ListProcess
    | ListInactiveProcess
    | AddDevice DeviceName DeviceMode
    | DeleteId Integer
    | DeleteDevice Device
    | DeleteAll
    | DeleteInactive
    | LoadFile FilePath
    | Exit
    | Terminate
    | Help
    | EnterKey
    | ClearScreen
    | Invalid String

parseInput :: String -> Command
parseInput input =
    case parse parseCommand "" input of
        Left   _  -> Invalid input
        Right cmd -> cmd

parseCommand :: Parser Command
parseCommand = padWhiteSpace (
    choice [ parseListProcess
           , parseAdd
           , parseDelete
           , parseLoad
           , parseExit
           , parseHelp
           , parseClear
           , parseTerminate
           , parseEnterKey
           ]) <* eof

parseListProcess :: Parser Command
parseListProcess = reserved "list" >>
    (  (reserved "inactive" $> ListInactiveProcess)
   <|>  pure ListProcess )

parseAdd :: Parser Command
parseAdd = reserved "add" >> AddDevice <$> parseDeviceName <*> parseMode
    where parseMode = choice [ char '2' $> Mode2, char '3' $> Mode3 ]

parseDelete :: Parser Command
parseDelete = reserved "delete" >> choice
    [ DeleteId       <$> (reserved "id"      *> natural)
    , DeleteDevice   <$> (reserved "device"  *> parseDeviceName)
    , DeleteAll      <$   reserved "all"
    , DeleteInactive <$   reserved "inactive" ]

parseLoad :: Parser Command
parseLoad = LoadFile <$> (reserved "load" >> parseFileName)

parseExit :: Parser Command
parseExit = (reserved "exit" <|> reserved "quit") $> Exit

parseHelp :: Parser Command
parseHelp = reserved "help" $> Help

parseClear :: Parser Command
parseClear = reserved "clear" $> ClearScreen

parseTerminate :: Parser Command
parseTerminate = reserved "terminate" $> Terminate

parseEnterKey :: Parser Command
parseEnterKey = pure EnterKey

parseDeviceName :: Parser String
parseDeviceName = identifier

parseFileName :: Parser String
parseFileName = stringLiteral




identifier :: Parser String
identifier = Token.identifier lexer

reserved :: String -> Parser ()
reserved = Token.reserved lexer

stringLiteral :: Parser String
stringLiteral = Token.stringLiteral lexer

natural :: Parser Integer
natural = Token.natural lexer

padWhiteSpace :: Parser a -> Parser a
padWhiteSpace x = whiteSpace *> x <* whiteSpace
    where whiteSpace = Token.whiteSpace lexer

lexer :: Token.GenTokenParser String w Identity
lexer = Token.makeTokenParser $ emptyDef
    { Token.commentLine   = "#"
    , Token.identStart    = letter
    , Token.identLetter   = alphaNum
    , Token.reservedNames =
        [ "list", "add", "delete", "device"
        , "id", "all", "load", "help", "clear"
        , "inactive", "quit", "exit", "terminate" ] }

